using System;
using MonoBrickFirmware;
using MonoBrickFirmware.Movement;
using MonoBrickFirmware.Display;
using MonoBrickFirmware.Sensors;
using MonoBrickFirmware.UserInput;
using System.Threading;
namespace Asdf
{
	class MainClass
	{
		public static void Scan()
		{
			MotorSync m = new MotorSync(MotorPort.OutB, MotorPort.OutD);
			WaitHandle wh;
			while (true)
			{
				wh = m.StepSync(50, -200, 300, true);
				wh.WaitOne();
				wh = m.StepSync(50, 200, 600, true);
				wh.WaitOne();
				wh = m.StepSync(50, -200, 300, true);
				wh.WaitOne();
				wh = m.StepSync(100, 0, 500, true);
				wh.WaitOne();
			}
		}

		public static void Main(string[] args)
		{
			ButtonEvents buts = new ButtonEvents();
			bool run = true;

			buts.EscapePressed += () =>
			{
				run = false;
			};

			MotorSync motori = new MotorSync(MotorPort.OutB, MotorPort.OutD);
			EV3IRSensor ir = new EV3IRSensor(SensorPort.In3, IRMode.Proximity);

			Thread bg = new Thread(Scan);
			bg.Start();
			while (run && ir.Read() > 80)
			{
				LcdConsole.WriteLine(ir.Read().ToString());
				Thread.Sleep(200);
			}
			bg.Abort();
			motori.Brake();
			motori.Off();
		}
	}
}