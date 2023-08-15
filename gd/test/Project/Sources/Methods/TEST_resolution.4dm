//%attributes = {}
$file:=Folder:C1567(fk resources folder:K87:11).file("sample.png")
$data:=$file.getContent()
BLOB TO PICTURE:C682($data; $picture)

$GD:=imagefrompicture($picture; IMG_PNG)

If ($GD.success)
	
	$options:=New object:C1471("resolutionX"; 300; "resolutionY"; 300)
	$PNG:=imagetopicture($GD.image; IMG_PNG; $options)
	
	If ($PNG.success)
		WRITE PICTURE FILE:C680(System folder:C487(Desktop:K41:16)+"1.png"; $PNG.image)
	End if 
	
End if 