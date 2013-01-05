jQuery.fn.reverse = function() {
    return this.pushStack(this.get().reverse(), arguments);
};

  
$(document).ready(function() {
	$(".dei-wrapper").each(function() {
	  if ($(this).find(".dei-wrapper").length) {
		$(this).addClass("dei-wrapper-has-leaf");
	  } else {
	    $(this).addClass("dei-wrapper-last-leaf");
	 }
	});
});
  
  
/* adds 3 divs for results*/
$(document).ready(function() {
  $(".dei-wrapper-0:last > .dei-result-box").html(
  "<span id=\"old-sum-last-label\">K rozdělení celkem: </span> <span id=\"old-sum-last\">_NEW_OVERALL_VALUE</span> _CURRENCY" + 
  "<span id=\"user-sum-last-label\">Váš návrh: </span> <span id=\"user-sum-last\"></span> _CURRENCY" + 
  "<span id=\"diff-last-label\"></span><span id=\"diff-last\"></span> _CURRENCY" + 
  "<span id=\"chart-last-label\">Obec:</span><div id=\"chart-first\"></div><span id=\"chart-last-label\">Váš návrh:</span><div id=\"chart-last\"></div><input class='form-submit' type='button' value='Aktualizovat'> ");
  $(".dei-wrapper-0:last > .dei-result-box").addClass(".dei-result-box-last");
});


/*disable "has-leaf" */
/*$(document).ready(function() {
  $(".dei-wrapper-has-leaf > .dei-user-value > div > input").attr("disabled","disabled");
});*/
/*disable the uppermost level*/ 
/*$(document).ready(function() {
  $(".dei-user-value-0 > div > input").attr("disabled","disabled");
});*/

/* calls recalculate on change*/
$(document).ready(function() {
  $("input").change(function() {
    recalculate_children ($(this).parent().parent());
	recalculate_sums ();
  });
});


/*set sums*/
$(document).ready(function() {
  add_sum ();
  recalculate_sums ();

});

/*set to numbers before exiting*/
$(document).ready(function() {
  $("#edit-save").click(function() {
   $(".dei-user-value > div > input").each(function() {
     $(this).parseNumber({format:"#,###", locale:"cz"});
   });
   $(".dei-old-value > div > input").each(function() {
     $(this).parseNumber({format:"#,###", locale:"cz"});
   });
  });
});

/*chart 2*/
function chart2() {
  //google.load("visualization", "1", {packages:["treemap"]});
  data0 = new google.visualization.DataTable();
  data0.addColumn('string', 'Child');
  data0.addColumn('string', 'Parent');
  data0.addColumn('number', 'Value');
  data0.addColumn('number', 'Change');
  
  data = new google.visualization.DataTable();
  data.addColumn('string', 'Child');
  data.addColumn('string', 'Parent');
  data.addColumn('number', 'Value');
  data.addColumn('number', 'Change');
  
  j = 0;  
  $(".dei-wrapper").each(function() {
    //old value
    old_value = $(this).children(".dei-old-value").children("div").children("input").parseNumber({format:"#,###", locale:"cz"});
    $(this).children(".dei-old-value").children("div").children("input").formatNumber({format:"#,###", locale:"cz"});
    //new value
    new_value = $(this).children(".dei-user-value").children("div").children("input").parseNumber({format:"#,###", locale:"cz"});
    $(this).children(".dei-user-value").children("div").children("input").formatNumber({format:"#,###", locale:"cz"});
    
    //old, new name, parent
    old_name = $(this).children(".dei-title").html();
    if (j>0) {
      old_parent = $(this).parent().children(".dei-title").html() + ': ' + $(this).parent().children(".dei-old-value").children("div").children("input").parseNumber({format:"#,###", locale:"cz"});
      $(this).parent().children(".dei-old-value").children("div").children("input").formatNumber({format:"#,###", locale:"cz"});
      
      new_parent = $(this).parent().children(".dei-title").html() + ': ' + $(this).parent().children(".dei-user-value").children("div").children("input").parseNumber({format:"#,###", locale:"cz"});
      $(this).parent().children(".dei-user-value").children("div").children("input").formatNumber({format:"#,###", locale:"cz"});
    } else {
      old_parent = null;
      new_parent = null;
    }
    
    data0.addRows([
      [old_name + ': ' + old_value,old_parent,parseInt(old_value),j]
    ]);
    data.addRows([
      [old_name + ': ' + new_value,new_parent,parseInt(new_value),j]
    ]);
    j++;
  })
  
  v0=new google.visualization.TreeMap(
    document.getElementById('chart-first')
        );
  v0.draw(data0, {maxPostDepth:3});

  v=new google.visualization.TreeMap(
    document.getElementById('chart-last')
        );
  v.draw(data, {maxPostDepth:3});

}

/*draw treemap*/
function drawVisualization() {

  // Create and draw the visualization.
  v=new google.visualization.TreeMap(
          document.getElementById('visualization')
        );
  v.draw(data, {maxPostDepth:2});
  // Pretend update data triggered and processed
  
  //google.visualization.events.addListener(v, 'onmouseover', update);
  
}


/*chart*/
function chart() {
  var user_ar = [];
  var old_ar = [];
  var label_ar = [];
  var color2_ar = ["adadad","7b48ff","00aef8","86ff21","efcd00","ff9718","ff0721","6e3339","9d00ef"];
  var color1_ar = ["d0cfcf","a799d1","63c8f0","b5fc7a","decc57","e4b06b","e65f6e","f3bac0","9a999b"];
  var sum_user = 0;
  var sum_old = 0;
  var i=0;
  $(".dei-wrapper-last-leaf").each(function() {
	user_ar[i] = $(this).children(".dei-user-value").children("div").children("input").parseNumber({format:"#,###", locale:"cz"});
	sum_user += user_ar[i];
	old_ar[i] = $(this).children(".dei-old-value").children("div").children("input").parseNumber({format:"#,###", locale:"cz"});
	sum_old += old_ar[i];
	label_ar[i] = $(this).children(".dei-title").html();
	i++;
	$(this).children(".dei-user-value").children("div").children("input").formatNumber({format:"#,###", locale:"cz"});
	old_ar[i] = $(this).children(".dei-old-value").children("div").children("input").formatNumber({format:"#,###", locale:"cz"});
  });
  var mx = Math.max(sum_user,sum_old);
  var diff = Math.round(sum_user) - Math.round(sum_old);
  var str_user = "";
  var str_old = "";
  var str_label = "";
  var str_color1 = "";
  var str_color2 = "";
  for (var j=0;j<i;j++) {
    str_user += Math.round(user_ar[j]/mx*100,2) + ",";
	str_old += Math.round(old_ar[j]/mx*100,2) + ",";
	if (label_ar[j].length > 12) {
	  str_label += encodeURI(label_ar[j].substr(0,5) + "..") + "|";
	} else {
	  str_label += encodeURI(label_ar[j]) + "|";
	}
	str_color1 += color1_ar[j % (color1_ar.length)] + "|";
	str_color2 += color2_ar[j % (color2_ar.length)] + "|";
  }
  str_user = str_user.replace(/,$/,"");
  str_old = str_old.replace(/,$/,"");
  str_label = str_label.replace(/\|$/,"");
  str_color1 = str_color1.replace(/\|$/,"");
  str_color2 = str_color2.replace(/\|$/,"");
  if (diff > 0) {
	var chart = "<img src=\'http://chart.apis.google.com/chart?cht=pc&chs=400x220&chd=t:" + str_old + ",-" + Math.round(diff/mx*100) + "|" + str_user + "&chl=" + str_label + "|empty|" + str_label + "&chco=" + str_color2 + "," + str_color1 + "\' />";
  } else {
    var chart = "<img src=\'http://chart.apis.google.com/chart?cht=pc&chs=400x220&chd=t:" + str_old + "|" + str_user + "," + Math.round(diff/mx*100) + "&chl=" + str_label + "|" + str_label + "&chco=" + str_color2 + "," + str_color1 +  "\' />";
  }
  
  $("#chart-last").html(chart).fadeIn("slow");
  
  

  
}

/*recalcualate sums*/
/* .each + .chidren have a bug, cannot use .children("something > something-else") !! */
function recalculate_sums () {
  $(".dei-wrapper-has-leaf").reverse().each(function() {
    var user_sum = 0;
    $(this).children(".dei-wrapper").reverse().each(function() {
		/*$(this).children(".dei-user-value").children("div").children("input")*/
	    user_sum += $(this).children(".dei-user-value").children("div").children("input").parseNumber({format:"#,###", locale:"cz"});
	});
	$(this).children(".dei-user-value").children("div").children("input").val(user_sum);
  });
   $("#user-sum-last").html($(".dei-wrapper-0:last > .dei-user-value > div > input").parseNumber({format:"#,###", locale:"cz"}));
   format_numbers();
   chart2();
}

function recalculate_children ($el) {
  var i = 0;
  var children_sum = 0;
  $el.parent().children(".dei-wrapper").each(function() {
    i++;
    children_sum = children_sum + $(this).children(".dei-user-value").children("div").children("input").parseNumber({format:"#,###", locale:"cz"});
  });
  var children_diff = parseFloat($el.children("div").children("input").val().replace(/ /g,"")) - children_sum;
  var last_value = Number(children_sum) + Number(children_diff);
  /*alert(children_diff);*/
	/* recalculation */
	if (children_sum == 0) {
	  $el.parent().children(".dei-wrapper").each(function() {
	    $(this).children(".dei-user-value").children("div").children("input").val(parseFloat(children_diff/i));
	  });
	} else {
		$el.parent().children(".dei-wrapper").each(function() {
			$(this).children(".dei-user-value").children("div").children("input").val(
			  parseFloat($(this).children(".dei-user-value").children("div").children("input").val().replace(' ', '')) * parseFloat(last_value/children_sum)
			);
		});
	}
	
	/*recurision*/
   $el.parent().children(".dei-wrapper").each(function() {
     /*alert($(this).children(".dei-user-value").html());*/
     recalculate_children( $(this).children(".dei-user-value") ); 
   });
  /*alert($el.children("div").children("input").parseNumber({format:"#,###", locale:"cz"}));*/
  /*alert(i);*/
}

/*calculate sum old */
function add_sum () {

  /*$("#old-sum-last").html($(".dei-wrapper-0:last > .dei-old-value > div > input").val());*/
}

/*format numbers - hardcoded to czech for now */
function format_numbers () {
   

   var diff = Math.round($("#user-sum-last").parseNumber({format:"#,###", locale:"cz"})) - Math.round(   $("#old-sum-last").parseNumber({format:"#,###", locale:"cz"}));
  if (diff < 0) {
    $("#diff-last-label").html("Ještě můžete rozdělit: ");
	$("#diff-last").html(-diff);
    $("#diff-last").formatNumber({format:"#,###", locale:"cz"});
	$("#diff-last").removeClass("blue");
	$("#diff-last").removeClass("red");
	$("#diff-last").addClass("green");
  } else {
    if (diff == 0) {
		$("#diff-last-label").html("Jste přesně na rozpočtu: ");
		$("#diff-last").html(diff);
		$("#diff-last").formatNumber({format:"#,###", locale:"cz"});
		$("#diff-last").removeClass("red");
		$("#diff-last").removeClass("green");
		$("#diff-last").addClass("blue");
	} else {
		$("#diff-last-label").html("Přesáhl/a jste rozpočet o: ");
		$("#diff-last").html(diff);
		$("#diff-last").formatNumber({format:"#,###", locale:"cz"});
		$("#diff-last").removeClass("blue");
		$("#diff-last").removeClass("green");
		$("#diff-last").addClass("red");
	}
  }
   $("#user-sum-last").formatNumber({format:"#,###", locale:"cz"});
   $("#old-sum-last").formatNumber({format:"#,###", locale:"cz"});
   $(".dei-user-value > div > input").each(function() {
     $(this).parseNumber({format:"#,###", locale:"cz"});
	 $(this).formatNumber({format:"#,###", locale:"cz"});
   });
   $(".dei-old-value > div > input").each(function() {
     $(this).parseNumber({format:"#,###", locale:"cz"});
	 $(this).formatNumber({format:"#,###", locale:"cz"});
   });
}
