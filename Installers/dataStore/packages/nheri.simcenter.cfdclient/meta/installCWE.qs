function Component() {}

Component.prototype.createOperations = function()
{
    component.createOperations();

    if (systemInfo.productType === "windows")
    {
        component.addOperation("CreateShortcut", "@TargetDir@/CWE-Simulation-Tool.exe", "@StartMenuDir@/CWE Simulation Tool.lnk",
            "workingDirectory=@TargetDir@", "description=Start SimCenter CWE Client Tool");
    }
}
