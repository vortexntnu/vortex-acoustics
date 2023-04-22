#include "Arduino.h"
#include "arm_math.h"
#include "pulse_determination.h"
#include "unity.h"

/*
Cosine wave generated from Python/pulse_detction/test/pulse_determination.py
generate_cosine_wave().

Parameters:
    pulse length = 4 ms
    carrier frequency = 25 kHz
    sampling frequency = 427 kHz
    noise amplitude = 0.9
    noise variance = 5
*/
namespace pulse_determination {
float32_t COSINE_WAVE[1708] = {
    0.7020062,   2.7188883,  0.6845851,  -1.0477405, -1.824456, 1.4874636,  1.1812806,  -2.7206507, -3.6606524, -2.313306,  -2.2799873, -1.3671145, -3.0273764,  -1.1534117, 1.7931819,   0.5899217,   -1.7592102, -1.8548586, 0.16250853, 0.071365446, 1.0143479,   0.8353053,   -2.0730164, 3.5786717, -1.0255975, 0.046720706, -1.6740634,  1.871118,   0.3350679, 0.19609344, -4.3552027, -1.6436931, 0.9496789,   0.4154565,  0.14360254, 4.199994,   -3.7818096, 2.1325147, -1.4738101,  3.3857555,   5.326554,    0.7990733, -2.0476756,  0.8920766, -0.48430935, -1.0804363, -0.45522115, 0.036292057, 0.9049582,  3.8125255,  1.8583329, 0.8659207,  1.1599191,  -3.1653032, 4.14679,     0.16656688, 4.419554,   -2.845414,  -2.6055868, -0.5725064,  1.6946731, 1.2894609,  1.637547,  -0.98854274, -2.10053,   -4.462052,  -0.4362734, -0.6963257, 2.9475722,  1.0683382,  1.2016292,  3.9133482,  2.652229,  1.1067374,  0.1153815,  -0.23180829, 0.52756226,  2.3967385,   -0.81950825, -5.501674,  -2.7156932, -1.6912264, 0.9901776,  4.379866,   1.2613177,  0.74362046, 1.6668321, -0.05779138, 2.44394,    1.0393727,   0.110951,  -2.8342156, -2.1727588,  -4.438296,  0.16470468, 1.8338182,  -1.777917,  0.42291078, 2.337089,    0.61060095, 0.39362526, -1.2128409, 2.5314305,  -2.607846,  3.2194839,  3.4924986,  0.46308252, -3.2688859,  -0.43511224, -0.021326052, -2.3832865, -1.4141058, -1.2269208, -1.9337702, -2.08668,  3.9247725, 4.18978,    2.8660867,  1.816127,    1.3056875,   1.7421803,  2.0042434,   -0.37765574, 0.14437105,  2.8157094,  0.52804774, -0.04799737, -2.4675689,   -0.26488584, -5.086627, -4.0861163, 1.170951,   1.0939962,  -1.7373595,  4.3454976,   1.0948411,  2.248978,   -1.5172744, 6.776865,   0.970612,   -0.088825256, -1.6862099, 2.1444466,  0.08684691, -1.4890673, 2.385027,      0.9707843,  -0.6972016, -0.9706254, 2.159821,    1.6962031,   3.174125,    1.3976344, 4.394617,   0.29195198,  -0.618631,  2.5598829,  -0.24226041, 0.56022257, -1.5006375, -0.49089178, -2.3715036, -0.8627552, 0.951296,  -1.213204,  -0.7790385, 2.1592927,  2.7658322,  0.46882108, -1.1343205, 2.572473,    -1.0676221, 0.49935213, -0.8600234, -1.1423861, -2.7918606, -3.0486887,  -1.7638942,  -2.225516,  -2.576731,    3.896803,  -1.1361263, -1.5062906, -0.5921356,  1.5543675,  2.9440064,  3.137069,   3.9805975,   2.9798489,  3.1324487, 0.722931,   0.9821584, -0.892533, -2.7526991, 2.3424346,  -1.7306925,   -2.581912, -6.441542,  -2.604661,  -4.2671323, 2.9914083,   -2.7316742, -0.45796677, 2.289107,   1.4852741,  1.4499786,  2.0120108, 8.709465,   2.3416622,  -0.9056993, 0.12697294,  -1.8090197, 2.8928196, -0.7958718,  -1.8896369, -0.21449994, -0.9949365, -0.87993973, -0.0108866785, 0.8906159,  1.0357524,  -1.9811796, 2.7919698, 1.1376461,  -0.8868288, 0.16566452, 0.965815,  -1.657332,  2.6395664,  2.278737,   -1.3056935,  -3.1339173, 3.1811717,   -4.274048,  3.496867,   3.8851867,  3.3082526,   1.4012581,  2.903616,     -1.7288527,  0.61988,     -2.5528927, 1.7666965,   4.00179,    0.050323337, -4.9645386, 1.6205713, -0.51694334, 0.0033036121, -1.8464607, 0.52597374,  -1.6360422, -1.9425769, 1.9950588, 0.7585985,  0.64394635, 3.0923822,   1.4945936,  2.488353,    0.7627429,  -0.62244034, 0.8685706, -3.0210857, -1.9230433,  -0.71009445, -0.8296203, -2.4839745,  -0.818143, -0.6065734, 1.9530846,   1.3444037,  -0.6052623, 0.6407545,  1.4387094, 2.620937,   0.45964518, 8.15694,   4.9575906,  0.4164328,   1.9982138, -1.5901388, -0.12916894, -0.77952415, 1.9123615,  -4.2236915, -1.8826605, 0.18237248, -0.23741099, -3.1410444, -2.3577375, 4.144238,   3.04981,   0.6095224, -1.3573315, -2.971716,  -1.5104395,  -1.9077032, 3.41159,    -0.9063322, 0.82354224, -1.426318,  -2.7741735,   -1.8967389, 2.800871,    1.7833564,   2.4522965,   -0.11326979, 0.929248,   -0.59625286, 1.4999774, 1.2308366, 0.753845,  -1.5445479, -1.1839815, 1.8487903,  -2.522649,   -2.2385254, -1.9031315, -0.37156707, 1.892049,   0.77503276, -2.36668,   0.0729696, -1.447277,  2.8321552,   2.6329846,   4.0864277,  3.1840997, 3.4731042,  1.0484138, -3.0400758, 0.045192733, 0.6653598,   1.2568315,  1.7753707,    -2.661384,  -0.82648796, -1.1856732, 0.23308131, -0.2572318, 1.1268443,  -0.41191706, 1.5381075,  2.5184608,   4.5318174,   -1.0135175, 3.3080711,   -2.738378, -2.3107696,  -3.0562623, -0.58905816, 1.112811,  -0.5480636, -0.83382857, 1.8804011,  0.4395622,  -1.5520866,  1.703321,     -1.1752226, 0.69226587,  3.4700346,  2.1984916, -2.3322887, -0.9994362, -0.6888838, -0.99365956, 1.4222194,    0.2183156,  1.094543,  2.0874152,  -4.1793737, 0.86769354, -1.1695414,  -0.6799388,   4.083768,    2.1004224,  1.8483393,  0.63705593,  1.6706684,   4.442249,   2.5186331,  -1.7182697, -0.9751241, 0.10220256, -1.9595314, -2.5531793, -0.7220367,  -1.2871339, -0.085733615, -1.4958521, 1.2351421,   -0.851967,  1.5173765, 0.3300492,  2.916071,   2.8954256, 1.8049126,  0.25834644, -1.0328332,  -0.17928867, 0.28350294, 0.83484566,   -0.2115288, 1.5530024,  -0.7599113, -3.4880357, -2.1317403, 5.0145254,    1.7405785,  -0.8291236, -1.6154784,  -1.8319762, -2.596872,  -0.83043545, 0.09406147, -3.322505,   0.39144826, 0.5949982,  -2.6582131, -0.9583311, 1.2438248,   -4.130349, 1.2790946,  5.5536566,  -1.2573644,  -1.6272389, -1.3231164, 0.14918494,  0.26968068, -0.86490023, 1.7204907, 0.06902127, 0.29611784, 0.1305036,    2.2262442,  1.3788137,  1.2696162, -1.5145105, -5.171485,   -0.35161754, -0.57268095, -0.71526873, 1.3616631,  0.7564384, 2.5771935,   0.97588974, 1.6918148,  0.07936801, 2.2569778, -2.7171218, -0.2947723, -5.6954646, -3.6818595,  -2.7584834,  1.1861484,  1.8307258,   0.05534357, 2.7475934, -1.635352,   -5.2304764, 1.430644,    0.81251955, 0.010519862, 2.8257284, 3.311384,    -3.1290874, -0.9927925,  -0.6590874, -2.854053, -2.1623328, -1.5147959, 1.4498948,  1.1986023,  -3.162912, -2.971863,   -3.5228553, 3.425806,     1.332312,   4.3040514,  -1.0848311,  -3.3312454,  0.48575982, 1.2804663, 0.74915504,  -2.5488458, -2.0637267,  -0.505148,  -1.6376826, -4.4053226,  -2.479043,  -0.15107027, 2.3052857,  0.80753154,  4.381756,    -2.9730914, -1.8314012,  1.910203,  3.0399911,   3.039641,    1.0235004,  -2.1093652, 2.014104,  -0.0075972937, 2.6393967,  -2.2807257, 1.3598032,  -0.44890845, -2.355869,   -1.6544574,  -1.4092046,  -2.4154532, 1.2561089,  4.2672186,  5.062979,   1.9435849,  4.6848774,   -2.2609458,   -1.6429172,  0.43376625, 1.9558954,  -6.4530497, -2.3825397, -3.0685508, -4.233747,  3.0698617, 4.467878,  0.51395595, -0.62570065, 0.32947463, 2.350985,    3.2834864,   -2.5423071, 2.2628357,  -1.7406335, 1.6204143,  -3.9670484, -0.068941034, -1.2134855, 1.167248,   1.8458034,  -1.853091,  0.011450924, 1.9340856,  -2.1563118,  1.2824314,  -1.0940704,  -2.5523536, 1.0284842,  1.6212276,  0.5150513, -0.98924834, -0.12404699, 1.4043286,   0.6425344, -2.8529282, -1.2729378,  0.5595152,   -3.2834952, 1.7621136,  0.38959184, 3.0899732,  0.7171288,  4.463386,  -0.88136613, -1.034717,  1.7963732, 0.765121,  -0.44615713, -4.3591976, 4.420159,
    -0.7337828,  -4.259702,  -1.4781462, 2.8085022,  1.9396186, -2.4337418, -2.6123328, -1.8567406, 2.8534362,  -2.1855233, -0.886284,  -3.0778797, -0.37843347, -0.7079875, -0.22562777, -0.75180787, 2.8565316,  2.8411114,  1.868099,   -1.0955378,  -0.18591961, -2.7389438,  -3.7420242, 2.3252466, 0.17202927, -2.6400933,  0.5845219,   -0.9211511, 1.8789357, -4.728878,  3.2397766,  2.7490144,  0.040187325, 4.8037577,  -3.6481488, -3.190204,  1.4279082,  -3.014089, -0.80039054, 0.116636746, 0.40591776,  1.8408417, -0.18071125, 3.4774766, 1.1744162,   3.8006816,  2.8505902,   1.9178946,   -3.8486357, -4.9926496, 1.0402979, 0.1466654,  0.5754982,  0.99540955, -3.91759,    -3.3332071, -3.2823398, 0.63786244, 2.9501755,  3.473854,    3.0815966, 1.1199027,  2.8444629, 1.9562668,   -0.5471511, 3.1038883,  3.1003628,  2.234075,   -1.8591994, -0.9336664, -1.7641686, 0.63425773, 1.4719073, 0.16246316, 1.5976541,  -0.3452068,  2.658577,    0.36777496,  2.4328876,   0.46385777, 1.0486106,  1.9329426,  1.3865846,  -1.7327102, 0.12532005, -2.6704206, 1.1160737, 1.3736081,   3.0189605,  -1.8270054,  -2.596981, 1.3404709,  0.26175943,  -0.5301124, 0.52536935, 4.582109,   0.4422025,  -1.780633,  -0.68189913, 0.24349329, -0.3913693, 0.3189565,  2.6873744,  0.80397236, -2.6660876, -3.3302495, -0.7133615, -0.20074548, -2.2207825,  -0.77838624,  -2.9513702, 1.9476713,  2.1613238,  1.4910496,  3.2536595, 4.731371,  -0.6071082, 2.4517133,  1.0453182,   0.38805532,  -4.1658998, -0.50546175, 0.23225681,  0.9233147,   -0.7494498, 0.97283435, 0.24756658,  -5.073198,    1.0628331,   0.6060905, 4.578924,   0.16766073, -1.4353789, -1.3956366,  -0.25262445, 3.084133,   -0.6678705, 0.4008066,  -1.2212021, -3.0012443, -1.0568981,   0.34177113, -0.2822301, -2.0758154, 4.217281,   -0.0041234973, 1.4531337,  0.41876167, 0.45304242, -0.88348156, -0.28985453, -0.49379504, 0.4737224, -2.799966,  -0.90841126, -0.4201612, -0.1972264, -0.22007391, -2.5248067, -0.9378249, -2.3173928,  -3.5362647, 1.0180794,  0.4136958, 2.8227277,  2.8654473,  -0.9286911, -4.9293866, -4.309729,  1.7008939,  -0.48069525, -2.9179616, 0.8361479,  -3.7995737, 1.1627828,  -3.4426112, -0.35112435, 1.6593482,   -2.8475866, -0.109460354, 1.0334908, -2.054132,  -1.7709237, -0.24630447, 2.8340864,  -0.9269379, 1.1904736,  -2.5973728,  -3.3738005, 0.9615252, 0.32995558, 0.1720286, 1.9834423, -0.7690603, -2.1862557, -0.047066506, 2.3769393, 4.3812723,  0.9010458,  1.0330125,  -0.37419516, 1.991957,   -2.4442465,  -2.2167215, 1.9828796,  2.3614008,  1.0905255, -3.0044508, -1.867122,  -1.7078543, 2.8060253,   -1.5586823, 2.4591079, -0.28424007, 0.7053135,  -1.1541212,  1.8907591,  4.795971,    1.7836751,     -4.0263085, 2.1038191,  2.0690594,  -4.473109, -3.1515594, 0.5602016,  0.9621787,  1.3964005, 1.5933361,  -1.1526941, 0.98389494, 1.7131141,   -1.8249582, -0.73761034, 3.9761713,  3.5270195,  1.5311408,  -0.30451256, -2.7817736, 2.3475087,    -0.28642094, -0.17028394, -0.3864465, -0.35022086, -2.9665666, -4.5863023,  2.5973032,  2.3913238, 1.8093876,   -2.5116687,   -3.185453,  -0.31947622, 1.1385388,  -1.1444883, 2.2222183, -0.5262894, 2.717357,   -1.9583012,  -0.7188468, -0.42252046, 1.1424819,  0.2820003,   2.9916573, -3.0268788, 0.040588588, 2.6399963,   -3.343655,  -0.7612901,  2.8397267, 2.904309,   1.2945305,   -1.9884151, -2.7240093, -0.4308215, 0.746556,  -2.2587428, -1.0729467, -3.599597, -2.6619024, -0.29159614, -4.35208,  3.7444105,  3.6560774,   0.3002431,   0.7450489,  -0.881891,  -0.6702849, -2.826042,  2.3132594,   7.605556,   -5.1181507, -2.3504808, -4.871105, 2.1089792, 0.16394879, 2.1335247,  -0.71261626, -1.4453762, 0.8676329,  -2.2728808, -3.370515,  0.6676939,  -0.080424614, 2.9066782,  3.6743271,   -2.6230297,  -0.14423923, -3.2511203,  -2.2156963, 4.573114,    1.050148,  -3.766222, -1.452215, 2.8773568,  -3.3920481, -2.1490848, -3.0505126,  0.08244243, 2.3694108,  5.06644,     1.250852,   0.17556292, -2.1674526, 0.8641988, 2.4726648,  0.53703386,  -0.35661575, -2.3343017, -3.351604, -3.7782767, 0.8965238, 0.44752896, 1.6438625,   -0.38422042, -2.043197,  2.5164666,    -2.2515852, 2.9713514,   1.1513911,  -0.8497893, 2.1476557,  0.53616625, 1.0464759,   2.6098404,  -0.18650055, -0.05064997, -2.2753396, -0.65276134, -4.777318, -0.2083684,  1.1540678,  2.0666785,   1.317964,  1.168703,   2.387476,    -3.2379344, 2.1077318,  -0.48438945, 4.050464,     -2.1379623, -0.08691217, -0.5825202, -4.90972,  2.8193412,  -3.7109575, 1.0161414,  1.3173314,   0.058841247,  0.42051548, 0.5628216, -0.3546686, 0.88449776, 1.9602319,  -2.9538367,  -1.2569048,   -3.6598563,  -3.8180463, -0.7705863, -0.80714244, -1.1951272,  -2.178653,  0.77806264, -1.9402008, -1.4724292, 1.8741769,  3.4442995,  2.5969043,  3.0005472,   3.9925652,  -1.5296353,   1.097424,   -0.9766697,  -1.4568144, 0.5019237, 1.7344642,  -1.4048889, -2.839477, -2.1147113, -3.7974155, 2.6379237,   4.185493,    1.9960022,  -1.652018,    -1.9100589, 0.60999197, 1.541236,   1.2654145,  2.8971372,  0.3899325,    0.16458175, -4.8219566, 0.014839224, -3.687334,  -2.5238655, 2.0016923,   0.92783844, -0.4906534,  -2.8887136, -0.9922888, 3.227696,   0.6883636,  -0.07921877, 3.3716369, -1.7967696, -3.0303478, -0.6625077,  -1.2314384, 0.25019428, 3.909294,    -4.570424,  -3.6313813,  0.8001716, 3.2485952,  -3.1632369, 0.49705082,   -1.8432935, 1.9150428,  1.7186648, 2.8638875,  -0.37918207, -0.8896832,  6.7226415,   2.9685063,   0.44602224, -5.421928, -0.10432589, -1.9081297, -1.7837359, 4.896712,   1.2911199, 2.6003127,  0.06193712, -3.0551322, 4.175638,    -0.35094798, 2.7538629,  4.0654383,   1.1652485,  2.5306609, -0.85242915, -0.9147942, 1.8007073,   -2.935899,  0.87940127,  -2.718677, -0.57297164, -3.6615713, 0.038380377, -1.0887463, 3.2645223, 1.8837885,  -3.716117,  -1.6959641, -1.6784005, 4.0092983, 0.032077566, 0.97569597, -0.020674156, 2.6898081,  -5.222261,  -0.77221805, -0.79905367, -3.3382995, -1.743185, -1.0473603,  -2.7137408, -0.41983235, 0.80616146, 3.4492817,  3.200496,    3.1594806,  -0.7128164,  3.5097506,  0.7051475,   -0.63350827, -3.1590095, -2.6080241,  1.867097,  1.3845252,   -0.40964428, -0.9625051, 0.76517206, 0.5539091, 3.1230128,     0.27874652, 1.8786356,  1.8083832,  -0.8864153,  -1.5288215,  -2.1375325,  -0.65916765, -1.7309265, 0.21005864, -3.9823744, -4.4389725, -1.5268549, -1.7071804,  0.56879723,   -0.38270617, -2.503477,  -2.8204253, -3.0125637, 3.197173,   2.8777905,  2.2054017,  2.124739,  1.5215944, -5.360349,  1.3153238,   -4.7492146, 0.6028294,   -1.0645467,  0.23445107, -1.1861556, -3.2003217, -1.5438063, 0.7195621,  -1.8143773,   0.5137529,  -2.8829691, 1.7177397,  -3.0475712, -1.7782617,  5.327337,   -0.21868502, 4.272002,   4.014728,    0.42307994, -2.2556474, 0.15004274, 3.643712,  1.3793927,   1.4850008,   3.3509533,   1.547133,  -1.6452122, -0.34640867, -3.1432762,  2.9942057,  4.4773655,  3.9925961,  -1.4077996, -0.4101753, 1.2475412, -1.721008,   -1.2940207, 2.1446702, 0.320953,  -4.9834123,  -0.8613742, -1.9479203,
    -0.31437433, 0.16217417, 0.45749506, 0.13680467, 1.1323957, 1.6118842,  1.123796,   3.226014,   2.1168118,  0.24478395, -3.2244103, 2.052193,   -2.553393,   -1.0643561, 1.108011,    1.2230963,   -2.4334173, 0.28692785, -2.095508,  -3.6824436,  4.1634226,   -0.57946235, 1.1333596,  3.395744,  1.3915285,  -3.3637044,  -0.57221115, 3.3809166,  2.8096986, 0.6577414,  -3.1345363, -4.0315003, -1.2473654,  -3.3730798, 2.9361172,  -0.7823493, 0.7137545,  1.7222419, -1.0652742,  -0.37961328, 0.007448507, 1.1076752, 4.1421537,   3.2992346, -0.37890613, -3.157527,  -2.6757834,  -0.14606957, -3.6226888, 2.2033308,  2.2012196, -2.1906335, -0.6361447, 1.3005216,  -0.86937463, 3.030738,   3.767834,   5.58011,    3.309805,   -0.08567785, -4.2953,   -2.3421397, -2.553145, 1.2944207,   -1.7421887, 0.36181775, 0.31342855, -1.7853658, -1.2927254, -1.5998536, -1.1270378, 3.6469927,  -1.213324, -2.9459245, 0.87636185, 0.31934935,  -0.26556286, -0.22071485, 0.39599285,  -2.588831,  -3.6380124, 0.5702441,  -1.2854286, -0.6111006, 3.8997078,  -1.3410009, 5.300957,  1.9640481,   -2.3732493, -0.48571295, 3.4425828, 4.205382,   -0.40700376, 1.0869861,  0.3524896,  -0.5332948, 0.31366912, -4.294421,  0.4530993,   -1.3415233, -0.6114474, -3.1463091, -1.4027063, -1.0541369, 0.5345127,  3.0283017,  1.8913656,  -3.1576707,  -4.1282997,  -2.9164343,   3.3447962,  3.5665903,  3.3173535,  -0.3091354, 2.8110514, -4.204252, -4.0686502, -0.8819621, -0.13885683, -0.13028583, 0.81391454, -0.46619254, 1.9652392,   -0.18829599, 3.5656803,  0.9403475,  -1.054169,   -0.043163806, 0.30793855,  0.1927952, -1.5129375, -2.1168501, -2.8531804, 0.008393191, -1.5794224,  -0.7706998, -2.1333408, 1.4935061,  -1.2987784, 3.1579704,  4.505435,     2.1425962,  -1.9628218, 1.5175188,  0.3949766,  1.7153302,     -0.8674341, -0.7734573, -1.7085196, -1.2315488,  0.9682545,   -2.1354246,  1.5420277, -1.7975208, 3.0099545,   -1.9810028, -1.5213085, 2.562081,    1.5176389,  1.5606663,  0.6455673,   2.9788876,  -1.1596516, 2.4144864, -2.4074383, -3.6380944, 1.6851093,  -1.3970933, 2.284142,   -3.1691103, -1.8092996,  1.6017601,  -1.2707378, -4.703915,  -0.7806056, 4.6952,     1.8495044,   -0.48234522, -1.5643535, 0.8542769,    1.5906763, 0.96053714, -1.070777,  -0.4733354,  -0.2749412, -2.1837134, -0.1391308, -0.17901114, 3.8529623,  2.3007052, -0.9289801, 1.9968331, 3.3126588, 0.3221299,  3.118174,   -1.0282435,   4.1339517, -0.6721579, -0.6884616, -5.6774178, -1.9073404,  -2.451029,  1.4196066,   -1.7933822, 0.17521237, -0.8207581, 5.815313,  1.1486421,  -0.9241873, 4.643419,   -0.27267453, 1.1319674,  1.8711559, -4.640392,   0.71193296, -2.0774825,  0.25530052, -1.2436103,  -2.9260945,    -3.006301,  -3.9217894, -3.5023482, 1.1775639, -1.9230711, 4.005662,   0.22290081, 3.6914315, -1.3787766, 1.4770217,  0.66494507, -0.46884307, 0.3470082,  1.0073502,   -1.5989257, -2.2739918, -1.3149171, 0.13223083,  0.5079973,  -0.053165477, -1.0081189,  -1.8080469,  0.53186876, 1.3337891,   1.1499598,  3.636718,    0.04989129, 1.2487166, -0.38993165, -1.727303,    -6.437255,  0.09784787,  -2.0842664, -0.8295711, 0.7752482, -2.2614038, 1.2848365,  -0.38483897, 3.2820802,  -0.65844953, 0.95424736, 1.0926678,   2.5618265, -1.23085,   3.3489416,   2.123931,    -2.1440604, -0.02312768, -1.276653, -3.2521796, 0.034942396, -3.6782575, 0.3973959,  -1.1935037, 0.8435042, -1.602029,  -0.6105784, 4.839523,  -2.111457,  3.693246,    3.7386293, 0.40241045, -1.9914652,  -2.8179936,  -2.0995467, -2.628709,  -1.5632463, 1.9008406,  -0.47844854, -0.789208,  -6.4262495, 0.6271127,  1.5056992, 3.1031218, 3.420178,   0.13229042, 1.0440193,   -1.7961745, 0.18540536, -1.4063413, 3.8865728,  0.97926736, -0.10138011,  -2.9248729, -0.48516658, -0.71680677, 0.6700909,   -0.30252248, 0.6350524,  -0.8701628,  1.7133168, -1.556702, 1.0609232, 4.2924566,  3.6700184,  0.66946864, -0.06135692, -1.9655144, -2.5567486, -2.4564984,  -3.8527794, -1.8641833, -1.9560933, -3.397485, -0.8143287, -0.33789745, 1.6804463,   0.2406491,  1.4644552, 0.6556546,  2.862851,  1.1714284,  0.22966379,  -0.22263132, -1.9279008, -0.028387848, 4.0817237,  -1.6181694,  -2.1643178, -3.722279,  1.9899753,  2.8100913,  -3.185049,   -3.0398872, -6.4939394,  -0.90912616, 0.1269929,  0.39510316,  4.4245,    -0.28238487, 1.8992732,  0.5798227,   0.7722769, 1.240241,   2.0316799,   -2.0225165, -3.5953553, -0.48975566, -0.030338526, -0.4364222, 0.60799056,  2.1559062,  4.124986,  5.7652345,  5.0990057,  -2.3544302, -3.1404927,  0.0016712632, -0.266678,  0.9458797, -0.3945683, -2.0367327, -2.4746861, -0.28266355, -0.053384542, -0.91534936, 0.07164746, 2.5320177,  1.19603,     -0.08377507, -3.9544547, 1.7633394,  2.6035225,  3.6624982,  1.743421,   0.51679754, -2.3953426, -0.45217296, 3.3985047,  0.72220165,   2.59441,    -0.16610008, -0.6827486, -3.25876,  -3.5630789, 0.39642736, 2.6981668, 0.518698,   2.1651387,  -0.36628503, -1.457879,   0.5167917,  -0.011340978, 1.5176368,  -2.827247,  -1.0892535, -2.2234282, -4.533881,  -0.024716021, 2.1772745,  -1.5185609, -2.9411018,  -3.3570893, 4.584189,   -2.588014,   3.335608,   -0.98289776, 3.2388425,  0.20629893, 4.1741366,  0.7038931,  -2.6888187,  3.0856235, 0.6138711,  -1.1443311, -0.65147364, -1.0620317, -0.9133227, -0.26389796, -4.0438814, 1.1374729,   2.2229486, 4.565879,   -1.6510109, -0.043481212, -1.6593988, -0.7020764, 3.2981043, 5.207249,   -0.23434855, 0.8082117,   -1.9983732,  -1.6916591,  -1.7631156, 2.555326,  0.6397992,   1.9027077,  2.1077013,  1.6841308,  0.7697296, 4.5383363,  0.97808427, 2.7988603,  -0.85185224, -2.7903748,  -3.4583008, -0.03828187, 0.36465764, 3.3619049, 3.855637,    -3.5385485, -0.85856074, -1.3319527, -3.0101845,  1.5072151, 0.44064558,  0.3073134,  1.1060525,   -2.4240565, 2.8109317, 0.33885124, 0.23723553, -3.2333522, 1.52136,    3.1475568, 0.16613156,  1.675439,   -2.0859385,   -1.6454599, -1.6434406, 2.7533236,   -1.7938387,  -0.4515533, -1.904652, -0.35860163, 1.11317,    0.72095466,  2.6195633,  -2.050234,  -0.12340601, -0.4281685, -3.2191415,  -0.3247173, -0.09693233, 1.880586,    -1.5289235, -0.06196795, 1.0268698, -0.21298233, 1.8693445,   0.3325241,  -1.7322328, 1.6425831, 0.17895474,    1.1558307,  -1.1262492, -1.2570959, -1.1504493,  -0.73787415, -0.20666689, -1.6742809,  -1.4698519, -4.589824,  1.3463221,  1.4091227,  2.8691583,  -0.11526528, 0.0026236936, 0.11950199,  0.86112696, -0.2901353, -1.0452216, 0.26968446, 0.7850103,  -1.1616488, 2.957213,  -3.206064, -3.3210902, 0.41880882,  0.5322817,  -0.79842156, -0.69474614, -1.8162234, 1.0544405,  -0.9728486, 1.8729428,  3.4604137,  0.7786607,    2.240304,   1.0091852,  -1.8374918, 2.0552297,  -2.6283324,  -0.4366169, -0.47909513, -4.0852056, -0.42276758, 2.3005743,  3.5022566,  2.5025895,  2.5067,    -4.376304,   2.204801,    -0.47309914, 3.9122968, 0.11853697, 2.7859337,   -0.77221185, -2.5376868, -1.0785336, -1.8679672, -3.4886076, -1.266914,  2.3683162, -0.8835571,  1.218209,   0.1372898, -1.179711, 2.320037,
};
float32_t EXPECTED_CARRIER_FREQ = 25.019531;

void test_short_time_fourier_transform() {
    uint32_t pulseLength = 4; // [ms]

    float32_t* pulse = new float32_t[pulseLength * SAMPLING_FREQUENCY];
    for (uint32_t i = 0; i < pulseLength * SAMPLING_FREQUENCY; i++) {
        pulse[i] = COSINE_WAVE[i];
    }

    float32_t* fft = new float32_t[FFT_SIZE];
    arm_status status = shortTimeFourierTransform(pulse, pulseLength, fft);
    TEST_ASSERT_TRUE(status == ARM_MATH_SUCCESS);

    float32_t computedCarrierFrequency = computeCarrierFrequency(fft);

    float32_t tolerance = static_cast<float32_t>(SAMPLING_FREQUENCY) / FFT_SIZE;
    TEST_ASSERT_FLOAT_WITHIN(tolerance, EXPECTED_CARRIER_FREQ, computedCarrierFrequency);

    delete[] pulse;
    delete[] fft;
}
} // namespace pulse_determination
