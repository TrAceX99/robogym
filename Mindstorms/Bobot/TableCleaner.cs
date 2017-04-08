using System;
using MonoBrickFirmware;
using MonoBrickFirmware.Movement;
using MonoBrickFirmware.Display;
using MonoBrickFirmware.Sensors;
using MonoBrickFirmware.UserInput;
using System.Threading;
using MonoBrickFirmware.Display.Dialogs;

namespace TableCleaner
{
	class MainClass
	{
		public static class C // Konstante
		{
			public const sbyte scanTurnSpeed = 50;
			public const sbyte scanForwardSpeed = 100;
			public const uint scanTurnSteps = 300;
			public const uint scanForwardSteps = 600;
			public const uint irScanTreshold = 80;

			public const sbyte pickupForwardSpeed = 100;

			public const sbyte carryTurnSpeed = 100;
			public const sbyte carryForwardSpeed = 100;
			public const uint carryDistanceToBeacon = 10;
			public const short carryBeaconLocationTurnCoeff = 2;

			public const sbyte dropoffPullbackSpeed = 50;
			public const uint dropoffPullbackSteps = 150;
			public const sbyte dropoffTurnSpeed = 100;

			public const sbyte armCalibrationPower = 20; // Potrebno podesavanje
			public const ushort armCalibrationTime = 2000; // Potrebno podesavanje
			public const uint armClosedPos = 100; // Potrebno podesavanje
			public const uint armLiftedPos = 100; // Potrebno podesavanje
			public const sbyte armSpeed = 100;

			public const uint oneeightyInSteps = 800;
			public const short left = -200;
			public const short right = 200;
			public const uint stepsPerCm = 100; // Potrebno podesavanje
		}

		public enum ArmPosition : uint
		{
			Open = 0,
			Closed = C.armClosedPos,
			Lifted = C.armLiftedPos
		};

		static MotorSync motors = new MotorSync(MotorPort.OutB, MotorPort.OutD);
		static Motor armMotor = new Motor(MotorPort.OutA);
		static EV3IRSensor ir = new EV3IRSensor(SensorPort.In3, IRMode.Proximity);
		static EV3ColorSensor color = new EV3ColorSensor(SensorPort.In4);
		static WaitHandle motorWH;
		static ArmPosition armPos; // Polozaj ruke za dizanje
		 
		// Ovaj deo ce biti implementiran kada se zavrsi funkcija skeniranja duzine stola.
		static byte progress = 0; // Koliki deo stola smo presli (u ciklusima ScanMovementa)
		static byte tableLenght = 255; // Duzina stola (u ciklusima ScanMovementa)

		// Resetuje ruku na polozaj 0 (Open)
		static void ArmCalibrate()
		{
			armMotor.ResetTacho();
			armMotor.SetPower(C.armCalibrationPower);
			Thread.Sleep(C.armCalibrationTime);
			armMotor.Off();
			Thread.Sleep(500);
			armMotor.ResetTacho();
			armPos = ArmPosition.Open;
		}

		// Pomera ruku na odredjeni polozaj
		static void ArmMove(ArmPosition targetPos)
		{
			int armTacho = armMotor.GetTachoCount();
			int direction = Math.Sign((uint)targetPos - armTacho);
			uint steps = (uint)Math.Abs((uint)targetPos - armTacho);
			sbyte speed = (sbyte)(C.armSpeed * direction);

			motorWH = armMotor.SpeedProfile(speed, 0, steps, 0, true);
			motorWH.WaitOne();
			armPos = targetPos;
			if (armPos != ArmPosition.Lifted)
				armMotor.Off();
		}

		// Kretanje Bobota prilikom trazenja predmeta
		static void SearchMove()
		{
			while (true) {
				motorWH = motors.StepSync(C.scanTurnSpeed, C.left, C.scanTurnSteps, true);
				motorWH.WaitOne();
				motorWH = motors.StepSync(C.scanTurnSpeed, C.right, C.scanTurnSteps * 2, true);
				motorWH.WaitOne();
				motorWH = motors.StepSync(C.scanTurnSpeed, C.left, C.scanTurnSteps, true);
				motorWH.WaitOne();
				motorWH = motors.StepSync(C.scanForwardSpeed, 0, C.scanForwardSteps, true);
				motorWH.WaitOne();
				progress++;
			}
		}
		// Algoritam trazenja
		static bool Search()
		{
			Thread movement = new Thread(SearchMove);
			movement.IsBackground = true; // Background thread se gasi kada se main zavrsi
			movement.Start();

			progress = 0;
			while (ir.Read() > C.irScanTreshold && progress < tableLenght) {
				LcdConsole.WriteLine(ir.Read().ToString());
				if (color.Read() == 0)
					throw new Exception("Color sensor value is zero");
				Thread.Sleep(200);
			}

			movement.Abort();
			motors.Brake();
			if (progress < tableLenght)
				return false;
			else
				return true;
		}
		// Algoritam podizanja predmeta
		static void Pickup()
		{
			ArmMove(ArmPosition.Open);
			motorWH = motors.StepSync(C.pickupForwardSpeed, 0, (uint)(ir.Read() * C.stepsPerCm), true);
			motorWH.WaitOne();
			ArmMove(ArmPosition.Lifted);
		}

		static void Carry()
		{
			ir.Mode = IRMode.Seek;
			BeaconLocation beacon = ir.ReadBeaconLocation();

			motors.SetSpeed(C.carryTurnSpeed, (short)(C.right * Math.Sign (beacon.Location)));

			while (beacon.Location > 5 || beacon.Location < -5) {
				beacon = ir.ReadBeaconLocation();
			}
			while (beacon.Distance > C.carryDistanceToBeacon) {
				motors.SetSpeed(C.carryForwardSpeed, beacon.Location); // Potreban koeficijent
			}
			motors.Brake();
			Thread.Sleep(200);
		}

		static void Dropoff()
		{
			ArmMove(ArmPosition.Open);
			motorWH = motors.StepSync(C.dropoffPullbackSpeed, 0, C.dropoffPullbackSteps, true);
			motorWH.WaitOne();
			ArmMove(ArmPosition.Closed);
			motorWH = motors.StepSync(C.dropoffTurnSpeed, C.right, C.oneeightyInSteps, true);
			motorWH.WaitOne();
		}

		public static void Main(string[] args)
		{
			try {
				ButtonEvents buts = new ButtonEvents();
				buts.EscapePressed += () => {
					throw new Exception("Program terminated by escape button");
				};

				ArmCalibrate ();
				ArmMove (ArmPosition.Closed);
				color.Mode = ColorMode.Reflection;

				while (true) {
					if (Search()) {
						Pickup();
						Carry ();
						Dropoff ();
					} else {
						Carry ();
						break;
					}
				}
			} catch (Exception ex) {
				InfoDialog d = new InfoDialog(ex.Message, "Exception");
				d.Show();
			} finally {
				motors.Off();
				armMotor.Off();
				Thread.Sleep(1000);
			}
		}
	}
}