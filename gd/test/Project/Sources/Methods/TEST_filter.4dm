//%attributes = {}
$file:=Folder:C1567(fk resources folder:K87:11).file("sample.png")
$data:=$file.getContent()
BLOB TO PICTURE:C682($data; $picture)

$GD:=imagefrompicture($picture; IMG_PNG)

If ($GD.success)
	
	$options:=New object:C1471("filter"; IMG_FILTER_COLORIZE; "red"; 100; "green"; 1; "blue"; 100; "alpha"; 10)
	$GD:=imagefilter($GD.image; $options)
	
	$PNG:=imagetopicture($GD.image; IMG_PNG)
	
	If ($PNG.success)
		WRITE PICTURE FILE:C680(System folder:C487(Desktop:K41:16)+"colorize.png"; $PNG.image)
	End if 
	
End if 