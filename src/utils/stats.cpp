#include "stats.hpp"

void calculateStats(
        cv::Mat imgCenterBlue,                  // Input image with blue pixels in the center
        cv::Mat imgCenterYellow,                // Input image with yellow pixels in the center
        opendlv::proxy::GroundSteeringRequest groundSteeringReq,   // Ground steering request object
        bool isBlueLeft                         // Flag indicating if blue is on the left side
)
{

    int bluePixelAmount = 0;                // Variable to store the number of blue pixels
    int yellowPixelAmount = 0;              // Variable to store the number of yellow pixels
    bool hasBluePixels = false;             // Flag indicating presence of blue pixels
    bool hasYellowPixels = false;           // Flag indicating presence of yellow pixels
    bool hasValidGroundSteering = false;     // Flag indicating the validity of ground steering request
    static int blueTurnsCorrectCount = 0;    // Counter for correct blue turns
    static int yellowTurnsCorrectCount = 0;  // Counter for correct yellow turns
    static int blueTurnsOnlyGSRCount = 0;    // Counter for only ground steering request turning right
    static int blueTurnsOnlyAlgorithmCount = 0;  // Counter for only algorithm turning right
    static int yellowTurnsOnlyAlgorithmCount = 0;  // Counter for only algorithm turning left
    static int yellowTurnsOnlyGSRCount = 0;   // Counter for only ground steering request turning left
    static int noBlueNoTurnsCount = 0;       // Counter for no blue pixels and no turn
    static int noYellowNoTurnsCount = 0;     // Counter for no yellow pixels and no turn

    bluePixelAmount = cv::countNonZero(imgCenterBlue);   // Count the number of non-zero (blue) pixels

    // Check if there are enough blue pixels to turn right
    if (bluePixelAmount > 250)
    {
        hasBluePixels = true;                        // Set the hasBluePixels flag to true
    }

    // Determine if the ground steering request is indicating a right turn based on the flag isBlueLeft
    if (isBlueLeft)
    {
        if (groundSteeringReq.groundSteering() < 0)       // Check if the ground steering request is negative (right turn)
        {
            hasValidGroundSteering = true;                 // Set the hasValidGroundSteering flag to true
        }
    }
    else
    {
        if (groundSteeringReq.groundSteering() > 0)       // Check if the ground steering request is positive (right turn)
        {
            hasValidGroundSteering = true;                 // Set the hasValidGroundSteering flag to true
        }
    }

    // Increment the respective counters based on the conditions
    if (hasBluePixels && hasValidGroundSteering)
    {
        blueTurnsCorrectCount++;                        // Increment the counter for correct blue turns
    }
    if (hasBluePixels && !hasValidGroundSteering)
    {
        blueTurnsOnlyAlgorithmCount++;                   // Increment the counter for only algorithm turning right
    }
    if (!hasBluePixels && hasValidGroundSteering)
    {
        blueTurnsOnlyGSRCount++;                         // Increment the counter for only ground steering request turning right
    }
    if (!hasBluePixels && !hasValidGroundSteering)
    {
        noBlueNoTurnsCount++;                             // Increment the counter for no blue pixels and no turn
    }

    yellowPixelAmount = cv::countNonZero(imgCenterYellow);   // Count the number of non-zero (yellow) pixels

    // Check if there are enough yellow pixels to turn left
    if (yellowPixelAmount > 250)
    {
        hasYellowPixels = true;                           // Set the hasYellowPixels flag to true
    }

    hasValidGroundSteering = false;                         // Reset the ground steering request flag

    // Determine if the ground steering request is indicating a left turn based on the flag isBlueLeft
    if (isBlueLeft)
    {
        if (groundSteeringReq.groundSteering() > 0)       // Check if the ground steering request is positive (left turn)
        {
            hasValidGroundSteering = true;                 // Set the hasValidGroundSteering flag to true
        }
    }
    else
    {
        if (groundSteeringReq.groundSteering() < 0)       // Check if the ground steering request is negative (left turn)
        {
            hasValidGroundSteering = true;                 // Set the hasValidGroundSteering flag to true
        }
    }

    // Increment the respective counters based on the conditions
    if (hasYellowPixels && hasValidGroundSteering)
    {
        yellowTurnsCorrectCount++;                        // Increment the counter for correct yellow turns
    }
    if (hasYellowPixels && !hasValidGroundSteering)
    {
        yellowTurnsOnlyAlgorithmCount++;                   // Increment the counter for only algorithm turning left
    }
    if (!hasYellowPixels && hasValidGroundSteering)
    {
        yellowTurnsOnlyGSRCount++;                         // Increment the counter for only ground steering request turning left
    }
    if (!hasYellowPixels && !hasValidGroundSteering)
    {
        noYellowNoTurnsCount++;                             // Increment the counter for no yellow pixels and no turn
    }

    hasYellowPixels = false;                         // Reset the yellow flag
    hasValidGroundSteering = false;                   // Reset the ground steering request flag
}

void writePixels(float bluePixels, float yellowPixels, float groundSteeringReq, float calcGroundSteering)
{
    std::ofstream outputFile;
    outputFile.open("/host/res.csv", std::ios_base::app);   // Open the file in append mode
    outputFile << bluePixels << "," << yellowPixels << "," << groundSteeringReq << "," << calcGroundSteering << "\n";   // Write the pixel and steering values to the file
    outputFile.close();                                     // Close the file
}

void determineError(float groundSteeringReq1, float groundSteeringReq2)
{
    static float totalFrames = 0;                     // Counter for total frames
    static float correctFrames = 0;                   // Counter for correct frames

    float error = ((fabs(groundSteeringReq1 - groundSteeringReq2)) / fabs(groundSteeringReq1)) * 100;   // Calculate the error percentage

    if (groundSteeringReq1 == 0)
    {
        error = fabs(groundSteeringReq1 - groundSteeringReq2);    // Set the error to the absolute difference when groundSteeringReq1 is zero
    }
    std::cout << "groundSteeringRequest: " << groundSteeringReq1 << std::endl;    // Print the original ground steering request
    std::cout << "steeringWheelAngle: " << groundSteeringReq2 << std::endl;        // Print the calculated steering value
    std::cout << "Error: " << error << std::endl;                     // Print the error value

    // Check if the error is within the acceptable range and increment the correctFrames counter accordingly
    if (groundSteeringReq1 != 0 && error <= 30)
    {
        correctFrames++;                    // Increment the counter for correct steering frames
    }
    else if (groundSteeringReq1 == 0 && fabs(groundSteeringReq1 - groundSteeringReq2) <= 0.05)
    {
        correctFrames++;                    // Increment the counter for correct steering frames when the ground steering request is zero
    }

    totalFrames++;                          // Increment the total frames counter

    std::cout << "Frame Stats:" << correctFrames << "/" << totalFrames << "\t" << ((correctFrames / totalFrames) * 100) << " %" << std::endl;   // Print the frame statistics
}
