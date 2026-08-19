//%attributes = {}
$file:=Folder:C1567(fk resources folder:K87:11).file("sample.png")
$data:=$file.getContent()
BLOB TO PICTURE:C682($data; $picture)

$GD:=imagefrompicture($picture; IMG_PNG)

If ($GD.success)
	
	$color:=imagecolorclosest($GD.image; 0; 0; 0)
	$color:=imagecolorclosestalpha($GD.image; 0; 0; 0; 0)
	$color:=imagecolorclosesthwb($GD.image; 0; 0; 0)
	$color:=imagecolorexact($GD.image; 0; 0; 0)
	$color:=imagecolorexactalpha($GD.image; 0; 0; 0; 0)
	$color:=imagecolorresolve($GD.image; 0; 0; 0)
	$color:=imagecolorresolvealpha($GD.image; 0; 0; 0; 0)
	
End if 