function Component() {}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows")
    {
        component.addOperation("CreateShortcut", "@TargetDir@/CWE-Simulation-Tool.exe", "@StartMenuDir@/CWE-Simulation-Tool.lnk",
            "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/icons/NHERI-CWE-Icon.icns", "description=Start SimCenter CWE Client Tool");
    }
}
