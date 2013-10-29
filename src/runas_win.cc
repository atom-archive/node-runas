#include "runas.h"

#include <windows.h>

namespace runas {

bool Runas(const std::string& command,
           std::vector<std::string>& args,
           int options,
           int* exit_code) {
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

  std::string parameters;
  for (size_t i = 0; i < args.size(); ++i)
    parameters += args[i] + ' ';

  SHELLEXECUTEINFO sei = { sizeof(sei) };
  sei.fMask = SEE_MASK_NOASYNC | SEE_MASK_NOCLOSEPROCESS;
  sei.lpVerb = "runas";
  sei.lpFile = command.c_str();
  sei.lpParameters = parameters.c_str();
  sei.nShow = SW_NORMAL;

  if (options & OPTION_HIDE)
    sei.nShow = SW_HIDE;

  if (::ShellExecuteEx(&sei) == FALSE || sei.hProcess == NULL)
    return false;

  // Wait for the process to complete.
  ::WaitForSingleObject(sei.hProcess, INFINITE);

  DWORD code;
  if (::GetExitCodeProcess(sei.hProcess, &code) == 0)
    return false;

  *exit_code = code;
  return true;
}

}  // namespace runas
