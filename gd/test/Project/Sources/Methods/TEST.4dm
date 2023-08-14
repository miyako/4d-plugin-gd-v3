//%attributes = {}
$file:=Folder:C1567(fk resources folder:K87:11).file("sample.png")
$data:=$file.getContent()
BLOB TO PICTURE:C682($data; $picture)

$GD:=imagefrompicture($picture; IMG_PNG)

If ($GD.success)
	
	$PNG:=imagetopicture($GD.image; IMG_PNG)
	If ($PNG.success)
		WRITE PICTURE FILE:C680(System folder:C487(Desktop:K41:16)+"1.png"; $PNG.image)
	End if 
	
	$BMP:=imagetopicture($GD.image; IMG_BMP)
	If ($BMP.success)
		WRITE PICTURE FILE:C680(System folder:C487(Desktop:K41:16)+"1.bmp"; $BMP.image)
	End if 
	
	$GIF:=imagetopicture($GD.image; IMG_GIF)
	If ($GIF.success)
		WRITE PICTURE FILE:C680(System folder:C487(Desktop:K41:16)+"1.gif"; $GIF.image)
	End if 
	
	$WEBP:=imagetopicture($GD.image; IMG_WEBP)
	If ($WEBP.success)
		WRITE PICTURE FILE:C680(System folder:C487(Desktop:K41:16)+"1.webp"; $WEBP.image)
	End if 
	
	$TIFF:=imagetopicture($GD.image; IMG_TIFF)
	If ($TIFF.success)
		WRITE PICTURE FILE:C680(System folder:C487(Desktop:K41:16)+"1.tiff"; $TIFF.image)
	End if 
	
	$JPEG:=imagetopicture($GD.image; IMG_JPEG)
	If ($JPEG.success)
		WRITE PICTURE FILE:C680(System folder:C487(Desktop:K41:16)+"1.jpeg"; $JPEG.image)
	End if 
	
	$WBMP:=imagetopicture($GD.image; IMG_WBMP)
	If ($WBMP.success)
		WRITE PICTURE FILE:C680(System folder:C487(Desktop:K41:16)+"1.wbmp"; $WBMP.image)
	End if 
	
End if 