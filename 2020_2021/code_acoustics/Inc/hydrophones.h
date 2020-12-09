/**
 * @file
 * 
 * @brief Small library that implements the 
 * hydrophones for the AUV for Vortex NTNU
 * 
 * @note Could be improved by using the pose
 * from ROS. Should be implemented in the 
 * future
 * 
 * @note It is thought that the hydrophones are
 * placed in a form of a triangle. Therefore, we
 * have one on front starboard, one on front port
 * and one on the back in the middle
 * 
 *                    BOW
 *                      
 *      Port                       Starboard
 *                      
 *       HYD ''''''''''''''''''''' HYD
 *           '''''''''''''''''''''
 *           ''''''''' A '''''''''
 *           ''''''''' U '''''''''
 *           ''''''''' V '''''''''
 *           '''''''''''''''''''''
 *           '''''''''''''''''''''
 *           '''''''''HYD'''''''''
 * 
 *                   STERN
 */
#ifndef ACOUSTICS_HYDROPHONES_H
#define ACOUSTICS_HYDROPHONES_H

#include "triliteration.h"
#include "DSP.h"


namespace HYDROPHONES{

/**
 * @brief Globals that describes the
 * hydrophones position compared to the
 * center of the AUV
 */
TRILITERATION::Pos pos_hyd_port(-1, 0, 0);
TRILITERATION::Pos pos_hyd_starboard(1, 0, 0);
TRILITERATION::Pos pos_hyd_stern(0, -1, 0);


/**
 * @brief Class for the hydrophones. 
 * Combines the basic functions for
 * each hydrophone. Makes it a lot 
 * easier to implement
 * 
 * @warning Must be evaluated if this
 * class is really necessary.
 */
class Hydrophones{
private:
    /**
    * @brief Position of each hydrophone 
    */
    TRILITERATION::Pos pos;

    /**
    * @brief The lag calculated for the last sample
    */
    uint32_t last_lag;

    /**
     * @brief The maximum value detected 
     */
    float32_t* p_max_val;

    /**
     * @brief The index the maximum-value was detected on
     */
    uint32_t* p_idx;

    /**
     * @brief The intensity calculated for the last sample
     */
    float32_t last_intensity;

    /**
    * @brief The dataset for one hydrophone 
    */
    float32_t* p_data;

    /**
     * @brief The magnitude of the dataset
     */
    float32_t* p_mag_data;

public:
    /**
    * @brief Constructor and destructor 
    */
    Hydrophones(TRILITERATION::Pos pos);
    ~Hydrophones();
    
    /**
    * @brief Function to return the last calculated lag 
    */
    uint32_t get_lag() const { return last_lag; } 

    /**
    * @brief Function to return the current data-set 
    */
    float32_t* get_data() const { return p_data; }

    /**
     * @brief Function to return the magnitude data of
     * the current data-set
     */
    float32_t* get_mag_data() const { return p_mag_data; }

    /**
     * @brief Function to return the intensity
     */
    float32_t get_intensity() const { return last_intensity; }

    /**
    *  @brief Function to calculate everything we are
    * interested in. This includes:
    *   -FFT and IFFT
    *   -index for maximum value
    *   -maximum value of the signal
    *   -signal intensity
    */
    void analyze_data(float32_t* data_arr);
};

} // namespace HYDROPHONES

#endif // ACOUSTICS_HYDROPHONES_H



