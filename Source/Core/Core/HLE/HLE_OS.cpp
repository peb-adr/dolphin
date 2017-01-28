// Copyright 2008 Dolphin Emulator Project
// Licensed under GPLv2+
// Refer to the license.txt file included.

#include <memory>
#include <string>

#include "Common/CommonTypes.h"
#include "Common/Logging/Log.h"
#include "Common/MsgHandler.h"
#include "Common/StringUtil.h"
#include "Core/HLE/HLE_OS.h"
#include "Core/HLE/HLE_VarArgs.h"
#include "Core/HW/Memmap.h"
#include "Core/PowerPC/PowerPC.h"

namespace HLE_OS
{
enum class ParameterType : bool
{
  ParameterList = false,
  VariableArgumentList = true
};

std::string GetStringVA(u32 str_reg = 3,
                        ParameterType parameter_type = ParameterType::ParameterList);

void HLE_OSPanic()
{
  std::string error = GetStringVA();
  std::string msg = GetStringVA(5);

  PanicAlert("OSPanic: %s: %s", error.c_str(), msg.c_str());
  ERROR_LOG(OSREPORT, "%08x->%08x| OSPanic: %s: %s", LR, PC, error.c_str(), msg.c_str());

  NPC = LR;
}

// Generalized function for printing formatted string.
void HLE_GeneralDebugPrint(ParameterType parameter_type)
{
  std::string report_message;

  // Is gpr3 pointing to a pointer rather than an ASCII string
  if (PowerPC::HostRead_U32(GPR(3)) > 0x80000000)
  {
    if (GPR(4) > 0x80000000)
    {
      // ___blank(void* this, const char* fmt, ...);
      report_message = GetStringVA(4, parameter_type);
    }
    else
    {
      // ___blank(void* this, int log_type, const char* fmt, ...);
      report_message = GetStringVA(5, parameter_type);
    }
  }
  else
  {
    if (GPR(3) > 0x80000000)
    {
      // ___blank(const char* fmt, ...);
      report_message = GetStringVA(3, parameter_type);
    }
    else
    {
      // ___blank(int log_type, const char* fmt, ...);
      report_message = GetStringVA(4, parameter_type);
    }
  }

  NPC = LR;

  NOTICE_LOG(OSREPORT, "%08x->%08x| %s", LR, PC, SHIFTJISToUTF8(report_message).c_str());
}

// Generalized function for printing formatted string using parameter list.
void HLE_GeneralDebugPrint()
{
  HLE_GeneralDebugPrint(ParameterType::ParameterList);
}

// Generalized function for printing formatted string using va_list.
void HLE_GeneralDebugVPrint()
{
  HLE_GeneralDebugPrint(ParameterType::VariableArgumentList);
}

// __write_console(int fd, const void* buffer, const u32* size)
void HLE_write_console()
{
  std::string report_message = GetStringVA(4);
  if (PowerPC::HostIsRAMAddress(GPR(5)))
  {
    u32 size = PowerPC::Read_U32(GPR(5));
    if (size > report_message.size())
      WARN_LOG(OSREPORT, "__write_console uses an invalid size of 0x%08x", size);
    else if (size == 0)
      WARN_LOG(OSREPORT, "__write_console uses a size of zero");
    else
      report_message = report_message.substr(0, size);
  }
  else
  {
    ERROR_LOG(OSREPORT, "__write_console uses an unreachable size pointer");
  }

  NPC = LR;

  NOTICE_LOG(OSREPORT, "%08x->%08x| %s", LR, PC, SHIFTJISToUTF8(report_message).c_str());
}

std::string GetStringVA(u32 str_reg, ParameterType parameter_type)
{
  std::string ArgumentBuffer;
  std::string result;
  std::string string = PowerPC::HostGetString(GPR(str_reg));
  auto ap = parameter_type == ParameterType::VariableArgumentList ?
                std::make_unique<HLE::SystemVABI::VAListStruct>(GPR(str_reg + 1)) :
                std::make_unique<HLE::SystemVABI::VAList>(GPR(1) + 0x8, str_reg + 1);

  for (size_t i = 0; i < string.size(); i++)
  {
    if (string[i] == '%')
    {
      ArgumentBuffer = '%';
      i++;
      if (string[i] == '%')
      {
        result += '%';
        continue;
      }

      while (i < string.size() &&
             (string[i] < 'A' || string[i] > 'z' || string[i] == 'l' || string[i] == '-'))
      {
        ArgumentBuffer += string[i++];
      }
      if (i >= string.size())
        break;

      ArgumentBuffer += string[i];

      switch (string[i])
      {
      case 's':
        result += StringFromFormat(ArgumentBuffer.c_str(),
                                   PowerPC::HostGetString(ap->GetArgT<u32>()).c_str());
        break;

      case 'a':
      case 'A':
      case 'e':
      case 'E':
      case 'f':
      case 'F':
      case 'g':
      case 'G':
        result += StringFromFormat(ArgumentBuffer.c_str(), ap->GetArgT<double>());
        break;

      case 'p':
        // Override, so 64bit Dolphin prints 32bit pointers, since the ppc is 32bit :)
        result += StringFromFormat("%x", ap->GetArgT<u32>());
        break;

      case 'n':
        // %n doesn't output anything, so the result variable is untouched
        PowerPC::HostWrite_U32(static_cast<u32>(result.size()), ap->GetArgT<u32>());
        break;

      default:
        if (string[i - 1] == 'l' && string[i - 2] == 'l')
          result += StringFromFormat(ArgumentBuffer.c_str(), ap->GetArgT<u64>());
        else
          result += StringFromFormat(ArgumentBuffer.c_str(), ap->GetArgT<u32>());
        break;
      }
    }
    else
    {
      result += string[i];
    }
  }

  if (!result.empty() && result.back() == '\n')
    result.pop_back();

  return result;
}

}  // end of namespace HLE_OS
