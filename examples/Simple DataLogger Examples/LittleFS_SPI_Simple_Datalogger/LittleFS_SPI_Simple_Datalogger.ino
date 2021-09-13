/*
  LittleFS  datalogger
 
 This example shows how to log data from three analog sensors
 to an storage device such as a FLASH.
 
 This example code is in the public domain.
 */
#include <LittleFS_NAND.h>

// LittleFS supports creating file systems (FS) in multiple memory types.  Depending on the 
// memory type you want to use you would uncomment one of the following constructors

//LittleFS_SPIFlash myfs;  // Used to create FS on SPI NOR flash chips such as the W25Q16JV*IQ/W25Q16FV,
                         // for the full list of supported NOR flash see 
                         // https://github.com/PaulStoffregen/LittleFS#nor-flash

LittleFS_SPINAND myfs;  // Used to create FS on SPI NAND flash chips on a SPI port 
                          // such as SPI, SPI1, SPI2 etc.  For the full list of supported 
                          //  NAND Flash chips see https://github.com/PaulStoffregen/LittleFS#nand-flash
LittleFS_SPINAND nand4; 
LittleFS_SPINAND nand5; 
LittleFS_SPINAND nand6; 




//LittleFS_SPIFram myfs;  // Used to create FS on FRAM memory chips such as the FM25V10-G.  
                          // For the full list of supported chips see https://github.com/PaulStoffregen/LittleFS#fram

const int chipSelect = 3;  // digital pin for Flash or Fram chip CS pin to create FS on QSPI NAND flash chips located on the bottom of the T4.1 such as the W25N01G. for the full list of supported NAND flash see  https://github.com/PaulStoffregen/LittleFS#nand-flash
const int chipSelect4 = 4;
const int chipSelect5 = 5;
const int chipSelect6 = 6;


File dataFile;  // Specifes that dataFile is of File type

int record_count = 0;
bool write_data = false;

void setup()
{

  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    // wait for serial port to connect.
  }
  Serial.println("\n" __FILE__ " " __DATE__ " " __TIME__);

  Serial.print("Initializing LittleFS ...");

  // see if the Flash is present and can be initialized:
  // Note:  SPI is default so if you are using SPI and not SPI for instance
  //        you can just specify myfs.begin(chipSelect). 
  if (!myfs.begin(chipSelect, SPI)) {
    Serial.printf("Error starting %s\n", "SPI3 FLASH");
    //while (1) {
      // Error, so don't do anything more - stay stuck here
    //}
  }
  if (!nand4.begin(chipSelect4, SPI)) {
    Serial.printf("Error starting %s\n", "SPI4 FLASH");
    //while (1) {
      // Error, so don't do anything more - stay stuck here
    //}
  }
  if (!nand5.begin(chipSelect5, SPI)) {
    Serial.printf("Error starting %s\n", "SPI5 FLASH");
    //while (1) {
      // Error, so don't do anything more - stay stuck here
    //}
  }
  if (!nand6.begin(chipSelect6, SPI)) {
    Serial.printf("Error starting %s\n", "SPI6 FLASH");
    //while (1) {
      // Error, so don't do anything more - stay stuck here
    //}
  }


  
  Serial.println("LittleFS initialized.");
  
  menu();
  
}

void loop()
{ 
  if ( Serial.available() ) {
    char rr;
    rr = Serial.read();
    switch (rr) {
      case 'l': listFiles(); break;
      case 'e': eraseFiles(); break;
      case 's':
        {
          Serial.println("\nLogging Data!!!");
          write_data = true;   // sets flag to continue to write data until new command is received
          // opens a file or creates a file if not present,  FILE_WRITE will append data to
          // to the file created.
          dataFile = myfs.open("datalog.txt", FILE_WRITE);
          logData();
        }
        break;
      case 'x': stopLogging(); break;
      case 'd': dumpLog(); break;
      case '\r':
      case '\n':
      case 'h': menu(); break;
    }
    while (Serial.read() != -1) ; // remove rest of characters.
  } 

  if(write_data) logData();
}

void logData()
{
    // make a string for assembling the data to log:
    String dataString = "";
  
    // read three sensors and append to the string:
    for (int analogPin = 0; analogPin < 3; analogPin++) {
      int sensor = analogRead(analogPin);
      dataString += String(sensor);
      if (analogPin < 2) {
        dataString += ",";
      }
    }
  
    // if the file is available, write to it:
    if (dataFile) {
      dataFile.println(dataString);
      // print to the serial port too:
      Serial.println(dataString);
      record_count += 1;
    } else {
      // if the file isn't open, pop up an error:
      Serial.println("error opening datalog.txt");
    }
    delay(100); // run at a reasonable not-too-fast speed for testing
}

void stopLogging()
{
  Serial.println("\nStopped Logging Data!!!");
  write_data = false;
  // Closes the data file.
  dataFile.close();
  Serial.printf("Records written = %d\n", record_count);
}


void dumpLog()
{
  Serial.println("\nDumping Log!!!");
  // open the file.
  dataFile = myfs.open("datalog.txt");

  // if the file is available, write to it:
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  } 
}

void menu()
{
  Serial.println();
  Serial.println("Menu Options:");
  Serial.println("\tl - List files on disk");
  Serial.println("\te - Erase files on disk");
  Serial.println("\ts - Start Logging data (Restarting logger will append records to existing log)");
  Serial.println("\tx - Stop Logging data");
  Serial.println("\td - Dump Log");
  Serial.println("\th - Menu");
  Serial.println();
}

void listFiles()
{
  Serial.print("\n Space Used = ");
  Serial.println(myfs.usedSize());
  Serial.print("Filesystem Size = ");
  Serial.println(myfs.totalSize());

  printDirectory(myfs);
}

void eraseFiles()
{
  myfs.quickFormat();  // performs a quick format of the created di
  Serial.println("\nFiles erased !");
}

void printDirectory(FS &fs) {
  Serial.println("Directory\n---------");
  printDirectory(fs.open("/"), 0);
  Serial.println();
}

void printDirectory(File dir, int numSpaces) {
   while(true) {
     File entry = dir.openNextFile();
     if (! entry) {
       //Serial.println("** no more files **");
       break;
     }
     printSpaces(numSpaces);
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numSpaces+2);
     } else {
       // files have sizes, directories do not
       printSpaces(36 - numSpaces - strlen(entry.name()));
       Serial.print("  ");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}

void printSpaces(int num) {
  for (int i=0; i < num; i++) {
    Serial.print(" ");
  }
}
