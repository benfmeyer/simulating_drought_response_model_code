import xarray as xr
import numpy as np
import sys

print(sys.argv[1])

scenario = sys.argv[1]
basepath = '/gpfs/scratch/pr48va/ge79vik2/drought_heat_mip/all_new/' + scenario + '/'

pft_names = dict(
    BNE='Boreal needleleaved evergreen tree (shade-tolerant)',
    BINE='Boreal needleleaved evergreen tree (shade-intolerant)',
    BNS='Boreal needleleaved summergreen tree',
    TeNE='Temperate needleleaved evergreen tree',
    TeBS='Temperate broadleaved summergreen tree (shade-tolerant)',
    IBS='Temperate broadleaved summergreen tree (shade-intolerant)',
    TeBE='Temperate broadleaved evergreen tree',
    TrBE='Tropical broadleaved evergreen tree (shade-tolerant)',
    TrIBE='Tropical broadleaved evergreen tree (shade-intolerant)',
    TrBR='Tropical broadleaved raingreen tree',
    C3G='C3 Grass',
    C4G='C4 Grass'
)

compression_dict = {"zlib": True, "complevel": 1}


print('Reading in the files!')

# slicing this because some outputs unfortunately have 200 years (1900-2099) and we only want the last 100 years
aout_pft = xr.open_dataset(basepath + 'AnnuallyOut.nc', group='Pft-Out', decode_times=False).isel(Time=slice(-100,None))
aout_base = xr.open_dataset(basepath + 'AnnuallyOut.nc', group='Base', decode_times=False).isel(Time=slice(-100,None))
aout_patch = xr.open_dataset(basepath + 'AnnuallyOut.nc', group='Patch-Out', decode_times=False).isel(Time=slice(-100,None))

# mout_pft: time 1000, gc 100 gets no warning
# 1000 x 1000 gets warning, but is much faster I think
mout_pft = xr.open_dataset(basepath + 'MonthlyOut.nc', group='Pft-Out', decode_times=False, chunks={'Time': 1000}).isel(Time=slice(-1200,None))#, 'Gridcell': 1000})
mout_base = xr.open_dataset(basepath + 'MonthlyOut.nc', group='Base', decode_times=False, chunks={'Time': 1000}).isel(Time=slice(-1200,None))#, 'Gridcell': 1000})
mout_patch = xr.open_dataset(basepath + 'MonthlyOut.nc', group='Patch-Out', decode_times=False, chunks={'Time': 1000}).isel(Time=slice(-1200,None))#, 'Gridcell': 1000})

# dout_pft: time 1000, gc 1000 no warning
# worked for the total output with only 'Time': 1000
# TODO did it fail when using only 'Time': 1000 for the pft output? I think so.
dout_pft = xr.open_dataset(basepath + 'DailyOut.nc', group='Pft-Out', decode_times=False, chunks='auto').isel(Time=slice(-36500,None))
dout_base = xr.open_dataset(basepath + 'DailyOut.nc', group='Base', decode_times=False, chunks={'Time': 1000, 'Gridcell': 1000}).isel(Time=slice(-36500,None))
dout_patch = xr.open_dataset(basepath + 'DailyOut.nc', group='Patch-Out', decode_times=False, chunks={'Time': 1000, 'Gridcell': 1000}).isel(Time=slice(-36500,None))

###################################################################################

print('Lets go with preparing them for the outputs')

# Monthly physical variables
mout = mout_patch.copy()
mout['longitude'] = mout_base['Longitude'][mout['Gridcell']]
mout['latitude'] = mout_base['Latitude'][mout['Gridcell']]
mout = mout.set_index(Gridcell=["longitude", "latitude"])
mout = mout.unstack(fill_value=np.nan)
mout = mout.squeeze()  # removes stands dimension


mip_names_monthly={
    'total runoff': 'runoff',
    'total transpiration': 'aet',
    'total evaporation': 'aep'
}

if True:
    for lpj_var, mip_var in mip_names_monthly.items():
        print('Monthly', lpj_var, mip_var)
        mipout = mout[lpj_var][:, :, :].to_dataset() \
            .rename({'Time': 'time'}) \
            .rename({lpj_var : mip_var})
        mipout = mipout.fillna(np.nan)  # otherwise the output file says the fill value were Infinityf, which is not true
        mipout['time'].attrs['units'] = 'months since 2000-01-01'
        mipout.to_netcdf('LPJ-GUESS_' + scenario + '_' + lpj_var.replace(' ', '_') + '.nc', encoding={mip_var : compression_dict})




# FPC annually
if True:
    print('FPC annually')
    mout_pft2 = aout_pft.copy()
    mout_pft2['longitude'] = aout_base['Longitude'][mout_pft2['Gridcell']]
    mout_pft2['latitude'] = aout_base['Latitude'][mout_pft2['Gridcell']]
    mout_pft2 = mout_pft2.set_index(Gridcell=["longitude", "latitude"]) \
        .unstack(fill_value=np.nan) \
        .squeeze() \
        .rename({'Pfts_and_total': 'vegtype', 'fpc': 'pftfraction', 'Time': 'time'})
    mipout = mout_pft2['pftfraction'][:, 0:-1, :, :].to_dataset() \
        .fillna(np.nan)
    mipout['time'].attrs['units'] = 'years since 2000-01-01'

    mipout.attrs = {'PFT' + str(k): pft_names[v] for k, v in enumerate(mout_base['Pfts'].values[0:-1])}
    mipout.to_netcdf('LPJ-GUESS_' + scenario + '_pftfraction.nc', encoding={'pftfraction' : compression_dict})



# FPC
if True:
    print('FPC monthly')
    mout_pft2 = mout_pft.copy()
    print('copy done')
    mout_pft2['longitude'] = mout_base['Longitude'][mout_pft2['Gridcell']]
    mout_pft2['latitude'] = mout_base['Latitude'][mout_pft2['Gridcell']]
    mout_pft2 = mout_pft2.set_index(Gridcell=["longitude", "latitude"]) \
        .unstack(fill_value=np.nan) \
        .squeeze() \
        .rename({'Pfts_and_total':'vegtype', 'fpc': 'pftfraction_monthly', 'Time': 'time'})
    mipout = mout_pft2['pftfraction_monthly'][:, 0:-1, :, :].to_dataset() \
        .fillna(np.nan)
    mipout['time'].attrs['units'] = 'months since 2000-01-01'

    mipout.attrs = {'PFT' + str(k): pft_names[v] for k, v in enumerate(mout_base['Pfts'].values[0:-1])}
    print('write')
    mipout.to_netcdf('LPJ-GUESS_' + scenario + '_pftfraction_monthly.nc', encoding={'pftfraction_monthly' : compression_dict})


############################################################
# Daily outputs
mip_names_daily={
    'leaf area index': 'lai',
    'gpp': 'gpp',
    'npp': 'npp',
    #     'fire emissions per pft': 'cFire'
}


data = dout_pft.copy()
data['longitude'] = dout_base['Longitude'][data['Gridcell']]
data['latitude'] = dout_base['Latitude'][data['Gridcell']]
data = data.set_index(Gridcell=["longitude", "latitude"]) \
    .unstack(fill_value=np.nan) \
    .squeeze() \
    .rename({'Pfts_and_total': 'vegtype', 'Time': 'time'})

if True:
    for lpj_var, mip_var in mip_names_daily.items():
        print('Daily', lpj_var, mip_var)
        mipout_pfts = data[lpj_var][:, 0:-1, :, :].to_dataset() \
            .fillna(np.nan) \
            .rename({lpj_var: mip_var})
        print(1)
        mipout_pfts.attrs = {'PFT' + str(k): pft_names[v] for k, v in enumerate(mout_base['Pfts'].values[0:-1])}
    
        print(2)
        mipout_pfts.to_netcdf('LPJ-GUESS_' + scenario + '_' + mip_var + '_pft.nc', encoding={mip_var : compression_dict})
    
    
        print(3)
        mipout_total = data[lpj_var][:, 12, :, :].to_dataset() \
            .fillna(np.nan) \
            .rename({lpj_var : mip_var})
    
        print(4)
        mipout_total.to_netcdf('LPJ-GUESS_' + scenario + '_' + mip_var + '.nc', encoding={mip_var : compression_dict})


# Daily patch only (rH, fire, and NEP)

if True:
    mip_names_daily_patch={
        'c emissions from fire' : 'cFire_with_litter',
        'heterotrophic respiration' : 'rh'
    }

    dout = dout_patch.copy()
    dout['longitude'] = dout_base['Longitude'][dout['Gridcell']]
    dout['latitude'] = dout_base['Latitude'][dout['Gridcell']]
    dout = dout.set_index(Gridcell=["longitude", "latitude"])
    dout = dout.unstack(fill_value=np.nan)
    dout = dout.squeeze()  # removes stands dimension
    dout = dout.rename(mip_names_daily_patch)

    for lpj_var, mip_var in mip_names_daily_patch.items():
        print('Daily patch', lpj_var, mip_var)
        mipout = dout[mip_var][:, :, :].to_dataset()
        mipout = mipout.rename({'Time': 'time'})
        mipout = mipout.fillna(np.nan)  # otherwise the output file says the fill value were Infinityf, which is not true
    
        mipout.to_netcdf('LPJ-GUESS_' + scenario + '_' + mip_var + '.nc', encoding={mip_var : compression_dict})


    # NEP = NPP - rh
    print('NEP')
    npp = data['npp'][:, -1, :, :].to_dataset() \
        .fillna(np.nan)
    rh = dout['rh'][:, :, :].to_dataset().rename({'Time': 'time'}).fillna(np.nan)
    
    nep = npp.copy()
    nep['nep']=nep['npp']-rh['rh']
    del nep['npp']
    
    nep.to_netcdf('LPJ-GUESS_' + scenario + '_nep.nc', encoding={'nep' : compression_dict})

#############################################################################
# Yearly Outputs


if True:

    mip_names_yearly_with_pft = {
        'carbon mass in vegetation': 'cVeg',
        'carbon mass in leaves': 'cLeaf',
        'carbon mass in fine roots': 'cRoot',
        'carbon mass in wood': 'cWood'
    }
    
    data = aout_pft.copy()
    data['longitude'] = dout_base['Longitude'][data['Gridcell']]
    data['latitude'] = dout_base['Latitude'][data['Gridcell']]
    data = data.set_index(Gridcell=["longitude", "latitude"]) \
        .unstack(fill_value=np.nan) \
        .squeeze() \
        .rename({'Pfts_and_total': 'vegtype', 'Time': 'time'}) \
        .rename(mip_names_yearly_with_pft)
    
    for lpj_var, mip_var in mip_names_yearly_with_pft.items():
        print('Yearly with PFT', lpj_var, mip_var)
        mipout_pfts = data[mip_var][:, 0:-1, :, :].to_dataset() \
            .fillna(np.nan)
        mipout_pfts.attrs = {'PFT' + str(k): pft_names[v] for k, v in enumerate(mout_base['Pfts'].values[0:-1])}
        mipout_pfts.to_netcdf('LPJ-GUESS_' + scenario + '_' + mip_var + '_pft.nc', encoding={mip_var : compression_dict})
        mipout_total = data[mip_var][:, -1, :, :].to_dataset() \
            .fillna(np.nan).squeeze()
        mipout_total.to_netcdf('LPJ-GUESS_' + scenario + '_' + mip_var + '.nc', encoding={mip_var : compression_dict})
    
    print('csoil')
    data = aout_patch.copy()
    data['longitude'] = dout_base['Longitude'][data['Gridcell']]
    data['latitude'] = dout_base['Latitude'][data['Gridcell']]
    data = data.set_index(Gridcell=["longitude", "latitude"]) \
        .unstack(fill_value=np.nan) \
        .squeeze() \
        .rename({'Time': 'time'})
    mipout = data['carbon mass in soil pool'].to_dataset() \
            .fillna(np.nan).rename({'carbon mass in soil pool' : 'cSoil'}).to_netcdf('LPJ-GUESS_' + scenario + '_cSoil.nc', encoding={'cSoil' : compression_dict})
