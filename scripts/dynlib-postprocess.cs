using NAnt.Core;
using NAnt.Core.Attributes;
using NAnt.Core.Logging;
using EA.Eaconfig;
using EA.Eaconfig.Core;
using EA.Eaconfig.Build;
using EA.Eaconfig.Modules;
using EA.Eaconfig.Modules.Tools;

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.IO;

[TaskName("dynlib-postprocess")]
class DynLib_PostprocessOptions : AbstractModuleProcessorTask
{
    public override void Process(Module_Native module)
    {
        if (module.IsKindOf(EA.FrameworkTasks.Model.Module.DynamicLibrary))
        {
            if (module.Cc != null)
            {
                module.Cc.Options.Add("-fshort-wchar");
                module.Cc.Options.Add("-std=c++11");
                module.Cc.Options.Add("-stdlib=libc++");
                module.Cc.Options.Add("-mmacosx-version-min=10.7");
            }

            foreach (var dep in module.Dependents.Flatten(DependencyTypes.Build | DependencyTypes.Link))
            {
                //if (dep.IsKindOf(EA.FrameworkTasks.Model.Module.Library))
                {
                    var dep_mod = dep.Dependent as Module_Native;
                    if (dep_mod != null && dep_mod.Cc != null)
                    {
                        bool is_cpp_mod = !dep_mod.BuildType.Name.Equals("CustomizedLibraryC", StringComparison.OrdinalIgnoreCase)
                                       && !dep_mod.BuildType.Name.Equals("CLibraryMod", StringComparison.OrdinalIgnoreCase)
                                       && !dep_mod.BuildType.Name.Equals("CLibrary", StringComparison.OrdinalIgnoreCase);

                        if (is_cpp_mod)
                        {
                            dep_mod.Cc.Options.Add("-std=c++11");
                            dep_mod.Cc.Options.Add("-stdlib=libc++");
                            dep_mod.Cc.Options.Add("-mmacosx-version-min=10.7");
                        }

                        dep_mod.Cc.Options.Add("-fshort-wchar");
                    }

                }

            }
        }
    }
}
