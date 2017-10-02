#ifndef _COMMAND_INTERPRETER_HPP_
#define _COMMAND_INTERPRETER_HPP_

#include <string>
#include <functional>
#include <list>

/**
 * An interpreter for commands from the serial input
 */
class CommandInterpreter
{
public:
  /**
   * Type of a command function
   */
  typedef std::function<bool(std::string&)> commandFunc;

  /**
   * Structure for a command that could be interpreted by this class
   */
  typedef struct command_s {
    std::string command;    //!< Command to react on
    commandFunc func;       //!< Function to call when command appears
  } command_t;

  /**
   * Get instance of CommandInterpreter
   * @return Pointer to the instance
   */
  static CommandInterpreter *get();

  /**
   * Virtual destructor
   */
  virtual ~CommandInterpreter();

  /**
   * add a command to the interpreter
   * @param commandWord     command to react on
   * @param commandFunction function to call when commandWord was found
   */
  void addCommand(std::string commandWord, commandFunc commandFunction);

  /**
   * Delete a command from the interpreter
   * @param commandWord    Command to react on
   */
  void deleteCommand(std::string commandWord);

  /**
   * Interprete a string s
   * When a registered command word appears, the given callback is called
   * by this interpreter
   * @param s Input string to interprete
   */
  void interprete(std::string &s);

  /**
   * Get the command word of a string s
   * @param  s string to interprete
   * @return   command of string s
   */
  static std::string getCommandWord(std::string &s);

  /**
   * Get an argument of an input s
   * @param  s      Input to use
   * @param  arg    (output) argument content
   * @param  argNum argument number
   * @return        success
   */
  static bool getArgument(std::string &s, std::string &arg, uint8_t argNum);

  /**
   * Get the number of arguments
   * @param  s Input to use
   * @return   Number of arguments of s. (command word is not counted as an argument)
   */
  static uint8_t getNumArgument(std::string &s);

private:
  /**
   * Private constructor
   */
  CommandInterpreter();

  /**
   * CI command to get help
   * @param s command
   * @return  success
   */
  bool helpCommand(std::string &s);

  /**
   * CI command to reset the configuration
   * @param s line to interprete
   * @return  success
   */
  bool configResetCommand(std::string &s);

  /**
   * CI command to print the configuraion
   * @param  s line to interprete
   * @return   success
   */
  bool configPrintCommand(std::string &s);

  // Instance
  static CommandInterpreter  *s_Instance;         //!< Instance of singleton

  // Membervariables
  std::list<command_t>        m_CommandList;      //!< List of all registered commands
};

#endif /* _COMMAND_INTERPRETER_HPP_ */
