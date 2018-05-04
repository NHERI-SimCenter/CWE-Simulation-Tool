function Component() {}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows")
    {
        component.addOperation("CreateShortcut", "@TargetDir@/CFDClientProgram.exe", "@StartMenuDir@/SimCenterCWE.lnk",
            "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/icons/NHERI-CWE-Icon.icns", "description=SimCenter CWE");
    }
}
