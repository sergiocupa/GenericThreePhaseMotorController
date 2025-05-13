#include "main.h"
#include "three_phase_drive_runner.h"
#include "three_phase_drive_preparer.h"


static ThreePhaseDriveData Instance = {0};


int main(void)
{
	three_phase_drive_prepare(&Instance);
	three_phase_drive_run(&Instance);
}


