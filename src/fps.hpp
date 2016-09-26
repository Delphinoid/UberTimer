#ifndef FPS_HPP
#define FPS_HPP

class fps{

	private:
		float desiredFps;  // Fps to simulate (so the program can be running at 120 fps but still act the same as it would running at 60 fps)
		float capFps;	  // Fps to cap the program at
		unsigned int startTick;	// The millisecond the FPS handler was initialized on
		unsigned int frameTick;	// The millisecond the current frame started on
		unsigned int totalFrames;  // How many frames there have been since the FPS handler was initialized


	public:
		float actualFps;   // Fps the program is actually running at
		float fpsMod;	  // A special number used in certain calculations to keep the program "synced" with the desired fps instead of the actual fps

		void start(float dFps, float cFps);
		void update();
		void delay();

};

#endif
