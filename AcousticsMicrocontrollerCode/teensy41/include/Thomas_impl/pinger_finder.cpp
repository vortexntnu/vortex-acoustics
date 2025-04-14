#include <vector>

#include "corelation.h"
#include "tdoa.h"
#include "pinger_finder.h"
#include <memory>






// Pos find_pinger(std::vector<std::vector<float32_t>> hydrophone_value_array, std::vector<Pos> hydrophone_array, int sampling_frequency){

//     std::vector<float32_t> TDOA;

    
    
//     for (int i = 1; i < (hydrophone_value_array.size()); i++){
//         std::vector<float32_t> correlation = still_brute_force_but_better_best_crosscorelation_lag(hydrophone_value_array.at(0), hydrophone_value_array.at(i));
//         int peek_index = find_peak_index_centered(correlation);
        


//         float32_t time_difference = static_cast<float32_t>(peek_index)/sampling_frequency;
//         TDOA.push_back(static_cast<float32_t>(time_difference));
//     }
    
//     Pos pinger_pos = tdoa_multilateration(hydrophone_array, TDOA);

//     return pinger_pos;
// }


// Pos find_pinger_new(std::vector<std::vector<float32_t>>& hydrophone_value_array, std::vector<Pos>& hydrophone_array, int sampling_frequency, int hydrophone_amount){

//     int num_timedifferences = hydrophone_amount-1;

//     std::vector<float32_t> TDOA;

    
    
//     for (int i = 0; i < (num_timedifferences); i++){
//         std::vector<float32_t> correlation = crosscorelation(hydrophone_value_array.at(0).data(), hydrophone_value_array.at(i+1).data());
//         int peek_index = find_peak_index_centered(correlation);
        


//         float32_t time_difference = static_cast<float32_t>(peek_index)/sampling_frequency;
//         TDOA.push_back(static_cast<float32_t>(time_difference));

//     }
    
//     Pos pinger_pos = tdoa_multilateration(hydrophone_array, TDOA);

//     return pinger_pos;
// }

