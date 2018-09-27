$(function() {

	document.title = "AntiDysentery";
	$("#PinTitle").html("Wagon Status");
    $("#PinTitle2").html("Location data");
    var smoothie = new SmoothieChart({
        grid: {
            strokeStyle: 'rgb(0, 0, 125)',
            fillStyle: 'rgb(0, 0, 30)',
            lineWidth: 1,
            millisPerLine: 2500,
            verticalSections: 6,
        },
        millisPerPixel: 100,
        maxValue: 16,
        minValue: 0,
        labels: { fillStyle: 'rgba(255,255,255,0.69)', fontSize: 14 },
        horizontalLines: [
            { color: '#990000', lineWidth: 1, value: 9 },
            { color: '#990000', lineWidth: 2, value: 14.4 }
        ]

    });

    smoothie.streamTo(document.getElementById("HistoryChart"), 3000);
    smoothie.addTimeSeries(chartData.line1, { lineWidth: 2, strokeStyle: 'hsl(120,100%,50%)' });
    smoothie.addTimeSeries(chartData.line2, { lineWidth: 2, strokeStyle: 'hsl(120,100%,25%)' });

    $(".slider").on("change", function() {rangeChange(this); });

	function myResize() {
        var canvas = document.getElementById("HistoryChart");
        resize(canvas);
    }

    function resize(canvas) {  //Resize Smoothie
        var displayWidth = canvas.clientWidth;
        var displayHeight = canvas.clientHeight;
        if (canvas.width != displayWidth || canvas.height != displayHeight) {
			canvas.width = displayWidth;
            canvas.height = displayHeight;
        }
    }

    GetStatus();
    updateRangeValues();

    $.ajaxSetup({ timeout: 500 });
    setInterval(GetStatus, 3000);
    window.addEventListener("load", myResize);
    window.addEventListener("resize", myResize);
	
});


	function GetStatus() {
		$.getJSON("status.jsn", function(data) {
			$("#jsoninfo").text(JSON.stringify(data, null, 4));

			$("#mainVoltage").html(data.mainVoltage);
			$("#auxVoltage").html(data.auxVoltage);
			
			$("#chipResets").html(data.chipResets);
			$("#requestsPerMinute").html(data.requestsPerMinute);
			$("#totalRequests").html(data.totalRequests);
			
			$("#gpsSpeed").html(data.GpsSpeed);
			$("#gpsDir").html(data.GpsDir);
			$("#gpsAlt").html(data.GpsAlt);
			$("#gpsLat").html(data.GpsLat);
			$("#gpsLon").html(data.GpsLon);
			
			setButton("#btnMainLed1", data.pwrLed1);
			setButton("#btnMainLed2", data.pwrLed2);
			setButton("#btnMainFlame", data.pwrFlame);			
			setButton("#btnMainSound", data.pwrSound);
			setButton("#btnMainHeadlight", data.pwrHeadlight);
			setButton("#btnMainAux1", data.pwrAux);

			setButton("#btnSparkleFade", data.SparkleFade);
			setButton("#btnChaseFade", data.ChaseFade);
			setButton("#btnNoise", data.Noise);

			setLedButton("btnSparkle",    "btnSparkleColor",    "rngSparkle",    "rngSparkleHue",    "Sparkle" ,   data.Sparkle,    data.SparkleColorMode);
			setLedButton("btnChase",      "btnChaseColor",      "rngChase",      "rngChaseHue",      "Chase",      data.Chase,      data.ChaseColorMode);
			setLedButton("btnBackground", "btnBackgroundColor", "rngBackground", "rngBackgroundHue", "Background", data.Background, data.BackgroundColorMode);

			setButton("#btnFlame", data.Flame);
			setButton("#btnRandom", data.Random);

			$("#rngSparkle").val(data.SparkleValue);
			$("#rngSparkleHue").val(data.SparkleHue);
			$("#rngChase").val(data.ChaseDistance);
			$("#rngChaseHue").val(data.ChaseHue);
			$("#rngBackground").val(data.BackgroundValue);
			$("#rngBackgroundHue").val(data.BackgroundHue);
			$("#rngFade").val(data.Fade);
			$("#rngSpeed").val(data.Speed);
			$("#rngFlameOn").val(data.FlameOn);
			$("#rngFlameOff").val(data.FlameOff);
			updateRangeValues();

			chartData.line1.append(new Date().getTime(), data.mainVoltage);
			chartData.line2.append(new Date().getTime(), data.auxVoltage);

		});
	}

	function setButton(name, data){
		var btn = $(name);
		if (data==1) {$(name).addClass("on"); }else {$(name).removeClass("on");}
		if (name== "#btnFlame"){
			if (data ==1) {
				$(".notFlame").prop("disabled", true);
				$(".notFlame").addClass("disabled");
			} else {
				$(".notFlame").prop("disabled", false);
				$(".notFlame").removeClass("disabled");
			}
		}
	}

	function setLedButton(name, name2, name3, name4, name5, var1, var2){
		var btn = $("#"+name);
		var btnH = $("#"+name2);
		var rng = $("#"+name3);
		var rngH = $("#"+name4);
		if (var1 == 1) {
			btn.addClass("on");
			//rng.removeClass("off");
			if (btnH.hasClass("on")==false) btnH.addClass("on");
			document.getElementById(name).innerHTML= name5 +": "+ document.getElementById(name3).value;   //Set toggle button with value
			if (var2==0){  //Default
				if (btnH.hasClass("onHue")) btnH.removeClass("onHue");
				document.getElementById(name2).innerHTML = "Default";  //name Color Button
			}
			if(var2==1){  //hue Shift
				btnH.addClass("onHue");
				document.getElementById(name).innerHTML  = name5 + ": " + document.getElementById(name3).value;  //name Toggle Button
				document.getElementById(name2).innerHTML = "Solid Color: " + document.getElementById(name4).value;  //name Color Button
				var col = "hsl("+document.getElementById(name4).value + ", 100%, 50%)";  //format hsl txt from range value
				document.getElementById(name2).style.setProperty("--main-bg-color",  col);
			}
			if (var2==2){//Random Color
				btnH.addClass("onHue");
				var coln = Math.floor(Math.random() * 360)
				var col = "hsl("+ coln + ", 100%, 50%)";
				document.getElementById(name2).style.setProperty("--main-bg-color",  col);
				document.getElementById(name2).innerHTML = "Randomize: " + coln;
			}
			if (var2==3){//Pallet shift
				if (btnH.hasClass("onHue")) btnH.removeClass("onHue");
				document.getElementById(name2).innerHTML="Pallet Shift";
			}
			if (var2==4){//Pallet Rotate
				if (btnH.hasClass("onHue")) btnH.removeClass("onHue");
				document.getElementById(name2).innerHTML="Pallet Rotate";
				
			}
		} else {
			document.getElementById(name2).innerHTML="OFF";
			btn.removeClass("on");
			btnH.removeClass("on");
		}
	}

function rangeChange(rng){
	$.post("rngRequest", {main: rng.id, data: rng.value});
	setTimeout(function() {	GetStatus()}, 1000);
}		

function mainPress(btn) {
	$.post("btnRequest" , {main: btn.id});
	if ($(btn).hasClass("on")==false) {
		$(btn).addClass("on") 
	}else{
		$(btn).removeClass("on");
	}
	setTimeout(function() {	GetStatus()}, 1000);
}

function flamePress(btn) {
	$.post("btnRequest" , {flame: btn.id});
	setTimeout(function() {	GetStatus()}, 1000);
}

function ledPress(btn) {
	$.post("btnRequest" , {led: btn.id});
	setTimeout(function() {	GetStatus()}, 1000);
}

function updateRangeValues() {
    document.getElementById("rngFlameOnVal").innerHTML = document.getElementById("rngFlameOn").value;
    document.getElementById("rngFlameOffVal").innerHTML = document.getElementById("rngFlameOff").value;

	col = "hsl("+document.getElementById("rngSparkleHue").value + ", 100%, 50%)";
	document.getElementById("rngSparkleHue").style.setProperty("--main-bg-color",  col);
    document.getElementById("rngSparkleVal").innerHTML = document.getElementById("rngSparkle").value;
    document.getElementById("rngSparkleHueVal").innerHTML = document.getElementById("rngSparkleHue").value;

	col = "hsl("+document.getElementById("rngChaseHue").value + ", 100%, 50%)";
	document.getElementById("rngChaseHue").style.setProperty("--main-bg-color",  col);
    document.getElementById("rngChaseVal").innerHTML = document.getElementById("rngChase").value;
    document.getElementById("rngChaseHueVal").innerHTML = document.getElementById("rngChaseHue").value;

	col = "hsl("+document.getElementById("rngBackgroundHue").value + ", 100%, 50%)";
	document.getElementById("rngBackgroundHue").style.setProperty("--main-bg-color",  col);
    document.getElementById("rngBackgroundVal").innerHTML = document.getElementById("rngBackground").value;
    document.getElementById("rngBackgroundHueVal").innerHTML = document.getElementById("rngBackgroundHue").value;

    document.getElementById("rngFadeVal").innerHTML = document.getElementById("rngFade").value;
    document.getElementById("rngSpeedVal").innerHTML = document.getElementById("rngSpeed").value;

	if ($(btnMainFlame).hasClass("on")){
		document.getElementById("cannonInfo").innerHTML = "--=Cannons Armed=--";
		$(".btnFlame").prop("disabled", false);
		$(".btnFlame").removeClass("disabled");
		$(".btnFlame").addClass("on");
	}else{
		document.getElementById("cannonInfo").innerHTML = "--=Cannons Disarmed=--";
		$(".btnFlame").prop("disabled", true);
		$(".btnFlame").addClass("disabled");
		$(".btnFlame").removeClass("on");
	}

}

// Global ==--------------------------------------------------------------------------------------------------------------------
var chartData = {
    line1: new TimeSeries(),
    line2: new TimeSeries()
};

function saveChartData(event) {
    console.log("Saving Chart Data");
    if (window.localStorage && chartData) {
        window.localStorage['chartData'] = JSON.stringify(chartData);
    }
}

function loadChartData(event) {
    if (window.localStorage) {
        try {
            var data = JSON.parse(window.localStorage['chartData']);
            console.log("Loading Chart Data");
            chartData.line1.data = data.line1.data;
            chartData.line2.data = data.line2.data;
        } catch (err) {};
    }
}

window.addEventListener("beforeunload", saveChartData);
window.addEventListener("load", loadChartData);