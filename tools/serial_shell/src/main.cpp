#include <Arduino.h>

#include <newton_fastled.hxx>
#include <Logger.h>

/* -- Configuration definitions -- */
#define CONFIG_SERIAL_RATE 115200UL
#define CONFIG_LED_TYPE NEOPIXEL
#define CONFIG_LED_DATA_PIN D2
#define CONFIG_NUM_LEDS 30

/* -- LED Configuration -- */
#define LED_MAX_VOLTS 5
#define LED_MAX_MAMPS 850

/* -- Global variables -- */
NewtonFastLEDInterpreter<CONFIG_NUM_LEDS> NewtonInterpreter{};

void setup()
{
	// Initialize Serial
	Serial.begin(CONFIG_SERIAL_RATE);

	Serial.println();
	Serial.println(__DATE__);

	// Initialize Logger
	Logger.begin(&Serial,
#ifdef DEBUG
				 Level::ALL
#else
				 Level::I
#endif

	);
	Logger.log<Level::D>(F("Setup"), F("Serial com initialized"));

	// Initialize LED strip
	FastLED.setMaxPowerInVoltsAndMilliamps(LED_MAX_VOLTS, LED_MAX_MAMPS);
	FastLED.addLeds<CONFIG_LED_TYPE, CONFIG_LED_DATA_PIN>(NewtonInterpreter.PublicBuffer, CONFIG_NUM_LEDS);
	FastLED.clear();
	FastLED.show();
}

void loop()
{
	if (Serial.available() > 0)
	{
		// Get a command from Serial
		auto command = Serial.readString();

		// Parse command
		command.toUpperCase();

		{ /* _indexOf lifetime blocl */
			int _indexOf = 0;

			if ((_indexOf = command.indexOf('\n')) > 0)
				command.replace(_indexOf, '\0');

			if ((_indexOf = command.indexOf('\r')) > 0)
				command.replace(_indexOf, '\0');
		}

		command.trim();

		// Show the instruction
		Logger.log<Level::I>(F("CMD"), command);

		// Parse the instruction
		auto instruction = Newton_ParseInstructionLiteral(command.c_str());

		// Handle instruction
		NewtonInterpreter.Instruction(instruction);
		FastLED.show();

		// Show instruction load
		switch (instruction.instruction)
		{
		case PRISM_INSTRUCTION_LOADX:
			Logger.log<Level::I>(F("Newton"), String{F("Loaded X with value: 0x")} + String(NewtonInterpreter.Registers.X, 16));
			break;

		case PRISM_INSTRUCTION_LOADY:
			Logger.log<Level::I>(F("Newton"), String{F("Loaded Y with value: 0x")} + String(NewtonInterpreter.Registers.Y, 16));
			break;

		default:
			break;
		}
	}
}