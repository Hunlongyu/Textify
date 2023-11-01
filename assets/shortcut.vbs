Set oWS = CreateObject("WScript.Shell")
sLinkFile = oWS.ExpandEnvironmentStrings("%USERPROFILE%") & "\Desktop\Textify.lnk"
Set oLink = oWS.CreateShortcut(sLinkFile)
oLink.TargetPath = oWS.CurrentDirectory & "\Textify.exe"
oLink.WorkingDirectory = oWS.CurrentDirectory
oLink.Save

oWS.Run oLink.TargetPath
