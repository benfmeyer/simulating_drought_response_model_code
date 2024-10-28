#ifndef GUESS_CFNDEPINPUTCONST_H
#define GUESS_CFNDEPINPUTCONST_H


class CfNdepInputConst {
private:
    int netcdf_noy_id;
    int netcdf_nhx_id;

    enum NDEP_TYPE {NHX, NOY, N_NTYPES};

    double get_ndep(double lon, double lat, NDEP_TYPE ndep_type);
public:
    CfNdepInputConst();

    void init();
    double get_ndep_noy(double lon, double lat);
    double get_ndep_nhx(double lon, double lat);

    virtual ~CfNdepInputConst();
};



#endif //GUESS_CFNDEPINPUTCONST_H
