// Frame management for timing and synchronization

// Function to synchronize frame updates
function synchronizeFrameUpdates() {
    const frameNumber = scanvideo_frame_number(); // Get the current video frame number
    // Update data per frame in sync with video rendering
    updateData(frameNumber);
}

function updateData(frameNumber) {
    // Logic to update your data based on the current frame number
    console.log(`Updating data for frame: ${frameNumber}`);
}

// Call the synchronization function in your main loop
setInterval(synchronizeFrameUpdates, 1000 / 30); // Example for 30 FPS