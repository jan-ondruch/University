<script>
var allStates = $("svg.us");

allStates.on("click", "*", function() {

if($(this).hasClass("on")) {
  $(this).removeClass("on");
  removeFromForm($(this).attr('id'));
 } else { 
   $(this).addClass("on");
  writeIntoForm($(this).attr('id'));
}

} );

function writeIntoForm(name) {	document.getElementById(document.querySelector('[id^="answer"]').id).value += name+";";
}

function removeFromForm(name) {

  	var content = document.getElementById(document.querySelector('[id^="answer"]').id).value;
	var outPutContent = content.replace(name+";","");
	document.getElementById(document.querySelector('[id^="answer"]').id).value = outPutContent;
  
}

</script>