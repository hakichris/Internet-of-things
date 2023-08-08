<?php


//Procedural
include "config.php";



if(isset($_GET['insert']))
 {
    $SensorName=$_GET['SensorName'];
    $SensorType=$_GET['SensorType'];
    $averageSensorValue=$_GET['averageSensorValue'];
    $Location=$_GET['Location'];
    $MillisTime=$_GET['MillisTime'];
    $ActuatorType=$_GET['ActuatorType'];
    $ActuatorValue=$_GET['ActuatorValue'];
    
    $sql="INSERT INTO `IOT_Midserm` (`SensorName`,`SensorType`,`averageSensorValue`, `ActuatorType`,`ActuatorValue`, `Location`,`MillisTime`)
         VALUES ('$SensorName','$SensorType' ,'$averageSensorValue', '$ActuatorType','$ActuatorValue', '$Location','$MillisTime' )";

    $q=mysqli_query($con,$sql);
   

 
 if($q)
  echo "<h1>Insert was Successful </h1>";
 else
  echo "error";
 
 }
?>