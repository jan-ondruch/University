function cookie(name) 
		{
		    var c_name = encodeURIComponent(name) + "=";
		    var cookies = document.cookie.split(';');
		    for (var i = 0; i < cookies.length; i++) 
		    {
		        var cookie = cookies[i];
		        while (cookie.charAt(0) === ' ') cookie = cookie.substring(1, cookie.length);
		        if (cookie.indexOf(c_name) === 0) return decodeURIComponent(cookie.substring(c_name.length, cookie.length));
		    }
		    return null;
		}

		$(document).ready(function(){
    		$("#show").click(function()
		    {
		        $("#box").toggle();
		        var date = new Date();
		        if($("#box").is(":visible"))
		        {
		            var mode = 1;
		        }
		        else
		        {
		            var mode = 2;
		        }
		        document.cookie = encodeURIComponent("open_dialog") + "=" + encodeURIComponent(mode) + "; expires=" + date.setTime(date.getTime() + (30 * 24 * 60 * 60 * 1000)) + "; path=/";
		    });
		    if(cookie("open_dialog") == 1)
		    {
				$("#box").show();
		    }
		});