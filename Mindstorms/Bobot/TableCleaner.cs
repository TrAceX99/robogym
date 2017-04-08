using System;
using MonoBrickFirmware;
using MonoBrickFirmware.Movement;
using MonoBrickFirmware.Display;
using MonoBrickFirmware.Sensors;
using MonoBrickFirmware.UserInput;
using MonoBrickFirmware.Display.Dialogs;
using System.Threading;

namespace TableCleaner
{
	class Bobot
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

			public const int motorBrakeSleepTime = 200;
			//public const uint oneeightyInSteps = 800;
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
		static Motor lMotor = new Motor(MotorPort.OutB);
		static Motor rMotor = new Motor(MotorPort.OutD);
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
			LcdConsole.WriteLine("Kalibracija ruke...");
			armMotor.ResetTacho();

			armMotor.SetPower(-C.armCalibrationPower);
			Thread.Sleep(C.armCalibrationTime);
			armMotor.Off();

			Thread.Sleep(500);
			armMotor.ResetTacho();
			armPos = ArmPosition.Open;
			LcdConsole.WriteLine("Kalibracija završena");
		}

		// Pomera ruku na odredjeni polozaj
		static void ArmMove(ArmPosition targetPos)
		{
			LcdConsole.WriteLine("Pomeranje ruke na položaj {0}", targetPos.ToString());
			int armTacho = armMotor.GetTachoCount();
			int direction = Math.Sign((uint)targetPos - armTacho);
			uint steps = (uint)Math.Abs((uint)targetPos - armTacho);
			sbyte speed = (sbyte)(C.armSpeed * direction);

			motorWH = armMotor.SpeedProfile(speed, 0, steps, 0, true);
			motorWH.WaitOne();
			armPos = targetPos;
			if (armPos != ArmPosition.Lifted)
				armMotor.Off();
			LcdConsole.WriteLine("Pozicija motora ruke: {0}", armMotor.GetTachoCount().ToString());
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
			LcdConsole.WriteLine("Trazenje predmeta...");
			lMotor.ResetTacho(); // Resetuje tacho motora zbog okretanja na kraju
			rMotor.ResetTacho(); 
			Thread movement = new Thread(SearchMove);
			movement.IsBackground = true; // Background thread se gasi kada se main zavrsi
			movement.Start();

			progress = 0;
			while (ir.Read() > C.irScanTreshold && progress < tableLenght) {
				LcdConsole.WriteLine(ir.Read().ToString());
				if (color.Read() == 0)
				{
					movement.Abort();
					motors.Brake();
					throw new Exception("Vrednost senzora za boju je 0!");
				}
				Thread.Sleep(200);
			}

			movement.Abort();
			motors.Brake();
			Thread.Sleep(C.motorBrakeSleepTime);
			if (progress == tableLenght)
				return false;
			else
				return true;
		}
		// Algoritam podizanja predmeta
		static void Pickup()
		{
			LcdConsole.WriteLine("Predmet pronadjen");
			ArmMove(ArmPosition.Open);
			motorWH = motors.StepSync(C.pickupForwardSpeed, 0, (uint)(ir.Read() * C.stepsPerCm), true);
			motorWH.WaitOne();
			ArmMove(ArmPosition.Lifted);
		}

		static void Carry()
		{
			LcdConsole.WriteLine("Vracanje do IR beacona...");
			ir.Mode = IRMode.Seek;
			BeaconLocation beacon = ir.ReadBeaconLocation();

			motors.SetSpeed(C.carryTurnSpeed, (short)(C.right * Math.Sign (beacon.Location)));

			while (beacon.Location > 5 || beacon.Location < -5) {
				beacon = ir.ReadBeaconLocation();
			}
			while (beacon.Distance > C.carryDistanceToBeacon) {
				motors.SetSpeed(C.carryForwardSpeed, beacon.Location); // Potreban koeficijent
				beacon = ir.ReadBeaconLocation(); // Mozda potrebno Thread.Sleep
			}
			motors.Brake();
			Thread.Sleep(C.motorBrakeSleepTime);
			LcdConsole.WriteLine("Rastojanje do IR beacona: {0}", ir.ReadBeaconLocation().Distance.ToString());
		}

		static void Dropoff()
		{
			LcdConsole.WriteLine("Spustanje predmeta...");
			ArmMove(ArmPosition.Open);

			motorWH = motors.StepSync(-C.dropoffPullbackSpeed, 0, C.dropoffPullbackSteps, true);
			motorWH.WaitOne();
			ArmMove(ArmPosition.Closed);

			LcdConsole.WriteLine("Tacho levog motora: {0}", lMotor.GetTachoCount().ToString());
			LcdConsole.WriteLine("Tacho desnog motora: {0}", rMotor.GetTachoCount().ToString());
			// Ovaj deo treba da vrati Bobota na pocetnu rotaciju
			uint deltaTacho = (uint)(rMotor.GetTachoCount() - lMotor.GetTachoCount());
			sbyte speed = (sbyte)(C.dropoffTurnSpeed * Math.Sign(deltaTacho));
			motorWH = motors.StepSync(speed, C.right, deltaTacho, true);
			motorWH.WaitOne();
			Thread.Sleep(C.motorBrakeSleepTime);
			LcdConsole.WriteLine("Ciklus zavrsen!");
		}

		public static void Main(string[] args)
		{
			try {
				ButtonEvents buts = new ButtonEvents();
				buts.EscapePressed += () => {
					throw new Exception("Escape gumb je stisnut!");
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
						LcdConsole.WriteLine("Ceo sto je ociscen!");
						Carry ();
						break;
					}
				}
			} catch (Exception ex) {
				InfoDialog d = new InfoDialog(ex.Message, "Exception");
				d.Show();
				Thread.Sleep(2000);
			} finally {
				motors.Off();
				armMotor.Off();
				Thread.Sleep(500);
			}
		}
	}
}
