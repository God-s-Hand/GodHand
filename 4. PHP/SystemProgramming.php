<?php
?>
<!DOCTYPE html>
<html>
<head>
    <title>SystemProgramming</title>
	<script src="https://ajax.googleapis.com/ajax/libs/jquery/1.11.3/jquery.min.js"></script>
</head>

<style>
</style>

<body>
SystemProgramming
GODHAND
<section id="sec1">
</section>


</body>

<script>

	var xmlhttp = new XMLHttpRequest();
	var url = "http://202.30.23.243/index/FileTransferServer/server/fileIndex.php";

	xmlhttp.onreadystatechange = function() {
		if (xmlhttp.readyState==4 && xmlhttp.status==200){
			console.log(xmlhttp.responseText);
			myFunction(xmlhttp.responseText);
		}
	}
	xmlhttp.open("GET", url, true);
	xmlhttp.send();
	

	function myFunction(response) {
		var arr = JSON.parse(response);
		var out = "<table>";
		for(var i = 1; i < arr.length; i++) {
			out+="<tr><td>" + arr[i].number+ "</td><td><img src='"+"http://202.30.23.243/index/FileTransferServer/server/" + arr[i].filename +"' width='300/'></td></tr>";
			
		}
		out += "</table>"
		document.getElementById("sec1").innerHTML = out;
	}

</script>
  
</html>