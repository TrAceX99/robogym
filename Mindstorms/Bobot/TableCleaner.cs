using System;
using MonoBrickFirmware;
using MonoBrickFirmware.Movement;
using MonoBrickFirmware.Display;
using MonoBrickFirmware.Sensors;
using MonoBrickFirmware.UserInput;
using System.Threading;

namespace TableCleaner
{
	class MainClass
	{
		public static class C // Konstante
		{
			public const int scanTurnSpeed = 50;
			public const int scanForwardSpeed = 100;
			public const int scanTurnSteps = 300;
			public const int scanForwardSteps = 600;
			public const int irScanTreshold = 80;
			public const int left = -200;
			public const int right = 200;
		};
			
		static MotorSync motors = new MotorSync(MotorPort.OutB, MotorPort.OutD);
		static EV3IRSensor ir = new EV3IRSensor(SensorPort.In3, IRMode.Proximity);
		static bool run = true;

		static void Scan()
		{
			WaitHandle wh;
			while (true)
			{
				wh = motors.StepSync(C.scanTurnSpeed, C.left, C.scanTurnSteps, true);
				wh.WaitOne();
				wh = motors.StepSync(C.scanTurnSpeed, C.right, C.scanTurnSteps * 2, true);
				wh.WaitOne();
				wh = motors.StepSync(C.scanTurnSpeed, C.left, C.scanTurnSteps, true);
				wh.WaitOne();
				wh = motors.StepSync(C.scanForwardSpeed, 0, C.scanForwardSteps, true);
				wh.WaitOne();
			}
		}

		public static void Main(string[] args)
		{
			ButtonEvents buts = new ButtonEvents();
			buts.EscapePressed += () =>
			{
				run = false;
			};

			Thread bg = new Thread(Scan);
			bg.Start();
			while (run && ir.Read() > C.irScanTreshold)
			{
				LcdConsole.WriteLine(ir.Read().ToString());
				Thread.Sleep(200);
			}
			bg.Abort();
			motors.Brake();
			Thread.Sleep (100);
			motors.Off();
		}
	}
}