//All commands for 'master'
//COMMAND ARRAY ------------------------------------------------------------------------------
const commandList_t masterCommands[] = {
  {"set",           setHandler,   "Quick set commands. Help: \"set help\""},
  {"get",           getHandler,   "Quick get commands. Help: \"get help\""},
  {"print",         printHandler,   "Print items. Help: \"print help\""},
  {"writeflash",    writeflashHandler,   "Write to flash."},
  {"readflash",     readflashHandler,   "read from flash."},

};
/* Command handler template
  bool myFunc(Commander &Cmdr){
  //put your command handler code here
  return 0;
  }
*/
void initialiseCommander() {
  cmd.begin(&Serial2, masterCommands, sizeof(masterCommands));
  cmd.commandPrompt(OFF); //enable the command prompt
  cmd.echo(false);     //Echo incoming characters to theoutput port
  cmd.errorMessages(OFF); //error messages are enabled - it will tell us if we issue any unrecognised commands
  //Error messaged do NOT work for quick set and get commands

}
//These are the command handlers, there needs to be one for each command in the command array myCommands[]
//The command array can have multiple commands strings that all call the same function

bool setHandler(Commander &Cmdr) {
  //quickget function
  //Call quickSetHelp() first to handle any help command
  Cmdr.quickSetHelp();
  if ( Cmdr.quickSet("us", us_factor) ) Cmdr.println("us set to " + String(us_factor));
  if ( Cmdr.quickSet("a_off", a_off) )   Cmdr.println("a_off set to " + String(a_off));
  if ( Cmdr.quickSet("a_min", a_min) ) Cmdr.println("a_min set to " + String(a_min));
  if ( Cmdr.quickSet("a_max", a_max) ) Cmdr.println("a_max set to " + String(a_max));
  if ( Cmdr.quickSet("alpha", alpha) ) Cmdr.println("alpha set to " + String(alpha));
  if ( Cmdr.quickSet("discard", discard) ) Cmdr.println("discard set to " + String(discard));
  if ( Cmdr.quickSet("readflash", readflash) ) Cmdr.println("readflash set to " + String(readflash));
  if ( Cmdr.quickSet("flash_count", flash_count) ) Cmdr.println("flash_count set to " + String(flash_count));
  return 0;
}
bool getHandler(Commander &Cmdr) {
  //quickset function
  //Call quickSetHelp() first to handle any help command
  Cmdr.quickSetHelp();
  Cmdr.quickGet("us", us_factor);
  Cmdr.quickGet("a_off", a_off) ;
  Cmdr.quickGet("a_min", a_min);
  Cmdr.quickGet("a_max", a_max);
  Cmdr.quickGet("alpha", alpha) ;
  Cmdr.quickGet("discard", discard) ;
  Cmdr.quickGet("readflash", readflash) ;
  Cmdr.quickGet("flash_count", flash_count) ;
  return 0;
}


bool printHandler(Commander &Cmdr) {
  //quickset function
  //Call quickSetHelp() first to handle any help command
  Cmdr.quickSetHelp();
  if (Cmdr.quick("us")) Cmdr.println(us_factor);
  if (Cmdr.quick("a_off")) Cmdr.println(a_off);
  if (Cmdr.quick("a_min")) Cmdr.println(a_min);
  if (Cmdr.quick("a_max")) Cmdr.println(a_max);
  if (Cmdr.quick("alpha")) Cmdr.println(alpha);
  if (Cmdr.quick("discard")) Cmdr.println(discard);
  if (Cmdr.quick("readflash")) Cmdr.println(readflash);
  if (Cmdr.quick("flash_count")) Cmdr.println(flash_count);
  return 0;
}

bool writeflashHandler(Commander &Cmdr) {
  write_to_flash();
  Cmdr.println("Write flash");
  return 0;
}

bool readflashHandler(Commander &Cmdr) {
  read_from_flash();
  Cmdr.println("reading flash");
  return 0;
}
