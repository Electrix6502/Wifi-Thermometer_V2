function toggleButton(element) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/button?b="+element.id, true);
  xhr.send();
  loadButtonState();
}

function toggleButtonSw(element) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/buttonSw?b="+element.id, true);
  xhr.send();
  loadButtonStateSw();
}

function getButtonState(id) {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	  if (this.readyState == 4 && this.status == 200) {
		  if (this.responseText == '1'){
			  document.getElementById(id).style.backgroundColor = 'red';
		  } else {
			  document.getElementById(id).style.backgroundColor = '#007bff';
		  }
	  }
	};
	xhttp.open("GET", "/button?s="+id, true);
	xhttp.send();	
}

function loadButtonState() {
	for(let i = 0; i < 11; i++){
		getButtonState(i);
	}
}

function getButtonStateSw(id) {

	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	  if (this.readyState == 4 && this.status == 200) {
		  if (this.responseText == '1'){
			  document.getElementById(id).style.backgroundColor = 'red';
		  } else {
			  document.getElementById(id).style.backgroundColor = '#007bff';
		  }
	  }
	};
	xhttp.open("GET", "/buttonSw?s="+id, true);
	xhttp.send();	
}

function loadButtonStateSw() {
	for(let i = 0; i < 17; i++){
		getButtonStateSw(i);
	}
}	 
 
function getData(id){
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	  if (this.readyState == 4 && this.status == 200) {
		document.getElementById(id).value = this.responseText;
	  }
	};
	xhttp.open("GET", "/data?c="+id, true);
	xhttp.send();	
}

function getCheckbox(id){
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
	  if (this.readyState == 4 && this.status == 200) {
		  if (this.responseText == '1'){
			  document.getElementById(id).checked = true;
		  } else {
			  document.getElementById(id).checked = false;
		  }
	  }
	};
	xhttp.open("GET", "/data?c="+id, true);
	xhttp.send();	
}

function fillOptions(id, pwm, ms){
	var text = "";
    text += '<option value="39">Ausgeschaltet</option>';
	if(pwm == 1){
		text += '<option value="32">PWM 1 (GAS)</option>';
		text += '<option value="33">PWM 2 (LENK)</option>';
		text += '<option value="34">PWM 3 (A3)</option>';
		text += '<option value="35">PWM 4 (A4)</option>';
		text += '<option value="36">PWM 5 (A5)</option>';
	};
	text += '<option value="16">BUS 1</option>';
	text += '<option value="17">BUS 2</option>';
	text += '<option value="18">BUS 3</option>';
	text += '<option value="19">BUS 4</option>';
	text += '<option value="20">BUS 5</option>';
	text += '<option value="21">BUS 6</option>';
	text += '<option value="22">BUS 7</option>';
	text += '<option value="23">BUS 8</option>';
	text += '<option value="24">BUS 9</option>';
	text += '<option value="25">BUS 10</option>';
	text += '<option value="26">BUS 11</option>';
	text += '<option value="27">BUS 12</option>';
	text += '<option value="28">BUS 13</option>';
	text += '<option value="29">BUS 14</option>';
	text += '<option value="30">BUS 15</option>';
	text += '<option value="31">BUS 16</option>';
	if(ms == 1){
		text += '<option value="0">MS A 1</option>';
		text += '<option value="1">MS A 2</option>';
		text += '<option value="2">MS A 3</option>';
		text += '<option value="3">MS A 4</option>';
		text += '<option value="4">MS A 5</option>';
		text += '<option value="5">MS A 6</option>';
		text += '<option value="6">MS A 7</option>';
		text += '<option value="7">MS A 8</option>';
		text += '<option value="8">MS B 1</option>';
		text += '<option value="9">MS B 2</option>';
		text += '<option value="10">MS B 3</option>';
		text += '<option value="11">MS B 4</option>';
		text += '<option value="12">MS B 5</option>';
		text += '<option value="13">MS B 6</option>';
		text += '<option value="14">MS B 7</option>';
		text += '<option value="15">MS B 8</option>';
	};
	document.getElementById(id).innerHTML = text;
}

function fillSwitch(id){
	var text = "";
    text += '<option value="0">1000</option>';
    text += '<option value="1">1200</option>';
    text += '<option value="2">1400</option>';
    text += '<option value="3">1500</option>';
    text += '<option value="4">1600</option>';
    text += '<option value="5">1800</option>';
	text += '<option value="6">2000</option>';
    document.getElementById(id).innerHTML = text;
}

function fillBright(id){
	var text = "";
	text += '<option value="1">1 - MIN</option>';
	text += '<option value="2">2</option>';
	text += '<option value="3">3</option>';
	text += '<option value="4">4</option>';
	text += '<option value="5">5</option>';
	text += '<option value="6">6</option>';
	text += '<option value="7">7</option>';
	text += '<option value="8">8</option>';
	text += '<option value="9">9</option>';
	text += '<option value="10">10</option>';
	text += '<option value="11">11</option>';
	text += '<option value="12">12</option>';
	text += '<option value="13">13</option>';
	text += '<option value="14">14</option>';
	text += '<option value="15">15</option>';
	text += '<option value="16">16 - MAX</option>';
	document.getElementById(id).innerHTML = text;
}
