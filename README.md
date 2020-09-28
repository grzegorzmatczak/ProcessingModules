# ProcessingModules


List of filters:

    #include "filters/custom_none.h"
    #include "filters/opencv_bilateralfilter.h"
    #include "filters/opencv_blur.h"
    #include "filters/opencv_canny.h"
    #include "filters/opencv_color.h"
    #include "filters/opencv_gaussianblur.h"
    #include "filters/opencv_inrange.h"
    #include "filters/opencv_medianblur.h"
    #include "filters/opencv_morphologyoperation.h"
    #include "filters/opencv_resize.h"
    #include "filters/opencv_sobel.h"
    #include "filters/opencv_threshold.h"
    #include "filters/opencv_bitwisenot.h"
    #include "filters/opencv_regionofinterest.h"
    #include "filters/custom_addgaussiannoise.h"
    #include "filters/custom_adddron.h"

List of backgroundsubtractors:

    #include "subtractors/custom_median.h"
    #include "subtractors/custom_none.h"
    #include "subtractors/opencv_cnt.h"
    #include "subtractors/opencv_gmg.h"
    #include "subtractors/opencv_gsoc.h"
    #include "subtractors/opencv_knn.h"
    #include "subtractors/opencv_lsbp.h"
    #include "subtractors/opencv_mog.h"
    #include "subtractors/opencv_mog2.h"

List of Trackers:

    #include "trackers/custom_none.h"
    #include "trackers/opencv_trackers.h"

List of Compares:

    #include "compares/custom_none.h"
    #include "compares/vot_challenge.h"
    #include "compares/votcpp_challenge.h"

List od Adders:

    #include "adders/custom_none.h"
    #include "adders/opencv_add.h"
    #include "adders/opencv_addweighted.h"
    #include "adders/opencv_absdiff.h"
