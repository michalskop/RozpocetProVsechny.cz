<?php

/**
*
*/
function dei_allocator_title () {
  return '';
}

/**
*
*/
function dei_allocator () {

  //$form_ar = dei_allocator_form(&$form_state);
  //$out .= $form_ar['header'];
  $out = drupal_get_form(dei_allocator_form(&$form_state));
  //$out .= $form_ar['footer'];

 return $out;
}

//***********
function dei_allocator_form(&$form_state) {
	$view_name = 'user_allocator';
	$display_id = 'defaults';
	// we assume that the view specified by $view_name has $display_id as its display.

	//To get the view specified by $view_name
	$view = views_get_view($view_name);
	$view->set_display($display_id);

	$arg = explode('/',drupal_lookup_path('source','allocator/'.arg(1)));

	//Optional: to set the arguments
	$view->set_arguments(array($arg[1]));	//zmena

	$view->execute();
	
	/*$arg = explode('/',drupal_lookup_path('source','allocator/'.arg(1)));
	$query = "
		SELECT 
			n.title as title,
			cfdi.vid as vid,
			cfdi.nid as mlid,
			cfdi.field_dei_allocator_nid as allocator_nid,
			ctbc.field_dei_category_description_value as description,
			ctbc.field_dei_category_weight_value as weight,
			ctbc.field_category_parent_nid as plid,
			ctbc.field_dei_user_value_value as user_value,
			cfdpv.field_dei_previous_value_value as old_value
		FROM 
			node as n
		LEFT JOIN
			content_field_dei_allocator as cfdi
		ON n.vid = cfdi.vid
		LEFT JOIN
			content_type_budget_category as ctbc
		ON n.vid = ctbc.vid
		LEFT JOIN content_field_dei_previous_value as cfdpv
		ON n.vid = cfdpv.vid
		WHERE field_dei_allocator_nid = %d
		ORDER BY field_dei_category_weight_value
	";
	$q = db_query($query,array($arg[1]));
	while ($r = db_fetch_array($q)) {
	  $nid = $r['mlid'];
	  $php_array[$nid] = $r;
	  $php_array[$nid]['has_children'] = 0;
	  if (!($r['plid'] > 0)) {
		$root_array[] =& $php_array[$nid];
		$php_array[$nid]['plid'] = 0;
	  }
	}
	//print_r($php_array);
	foreach ($php_array as $pa) {
	  if ($pa['plid'] > 0) {
	    $php_array[$pa['plid']]['children'][$pa['mlid']] = $pa['mlid'];
		//echo $pa['plid'] . '<br/>';
	  }
	}*/
	//print_r($php_array);
	foreach ($view->result as $key => $result) {
	  $nid = $result->nid;
	  $php_array[$nid]['mlid'] = $result->nid;
	  $php_array[$nid]['weight'] = $result->node_data_field_dei_category_weight_field_dei_category_weight_value;
	  $php_array[$nid]['plid'] = $result->node_data_field_category_parent_field_category_parent_nid;
	  $php_array[$nid]['title'] = $php_array[$nid]['node_title'];
	  $php_array[$nid]['user_value'] = $result->node_data_field_category_parent_field_dei_user_value_value;
	  $php_array[$nid]['depth'] = 1;
	  $php_array[$nid]['description'] = $result->node_data_field_category_parent_field_dei_category_description_value;
	  $php_array[$nid]['has_children'] = 0;
	  $php_array[$nid]['title_callback'] = t;
	  $php_array[$nid]['old_value'] = $result->node_data_field_category_parent_field_dei_previous_value_value + 0;
	  $php_array[$nid]['allocator_title'] = $result->node_node_data_field_dei_allocator_title;
	  $php_array[$nid]['allocator_description'] = $result->node_node_data_field_dei_allocator_node_data_field_dei_description_field_dei_description_value;
	  //drupal_set_message(print_r($result->node_data_field_category_parent_field_dei_previous_value_value,1). print_r($php_array[$nid]['old_value'],1));
	  if (!($result->node_data_field_category_parent_field_category_parent_nid > 0)) {
		$root_array[] =& $php_array[$nid];
		$php_array[$nid]['node_data_field_category_parent_field_category_parent_nid'] = 0;
	  } else {
		$php_array[$result->node_data_field_category_parent_field_category_parent_nid]['children'][$nid] = $nid;
	  }
	}
	if (isset($root_array)) {
		foreach ($root_array as $ra) {
		  $final_array[] = recursor($php_array, $ra);
		}
	} else {
		drupal_set_message(t('No root budget category. Choose/add at least one. Or you do not have rights to access them.'));
	}
	drupal_set_message('<pre>' .  print_r($view->result,1). '</pre>');
	   if (isset($final_array)) {
		$form = _dei_allocator_tree_form($final_array);
		//drupal_set_message('<pre>' .  print_r($form,1). '</pre>');
		  $form['infotext'] = array(
				'#type' => 'fieldset',
				'#prefix' => '<div id="infotext-wrapper">', // This is our wrapper div.
				'#suffix' => '</div>',
			);
		/*$form['my_captcha_element'] = array(
		  '#type' => 'captcha',
		);*/
		  $form['save'] = array(
				'#type' => 'submit',
				'#value' => t('Save'),
			);
		}
	  //include custom js
	  _include_js_calc();
	  
	 return $form;

}
/**
*
*/
function _dei_allocator_tree_form($tree,&$form = array('#tree' => TRUE),$level = 0) {
//drupal_set_message('<pre>' .  print_r($tree,1). '</pre>');
  //static $form = array('#tree' => TRUE);
  foreach ($tree as $data) {
    $title = '';
    $item = $data['link'];
    // Don't show callbacks; these have $item['hidden'] < 0.
    if ($item && $item['hidden'] >= 0) {
      $mlid = 'mlid:'. $item['mlid'];
      $form[$mlid]['#item'] = $item;
      $form[$mlid]['#attributes'] = $item['hidden'] ? array('class' => 'menu-disabled') : array('class' => 'menu-enabled');
      $form[$mlid]['title']['#value'] = $item['title'] . ($item['hidden'] ? ' ('. t('disabled') .')' : '');
      //$form[$mlid]['#description'] = $item['description'];
	  $form[$mlid]['description'] = array(
        '#type' => 'fieldset',
		'#description' => $item['description'],
      );
      $form[$mlid]['old_value'] = array(
        '#type' => 'textfield',
        '#default_value' => $item['old_value'],
		'#value' => $item['old_value'],
		'#disabled' => TRUE,
		'#prefix' => '<div class="dei-old-value dei-old-value-'.$mlid.' dei-old-value-'.$level.'">',
		 '#suffix' => '</div>',
      );
	  $form[$mlid]['user_value'] = array(
        '#type' => 'textfield',
        '#default_value' => $item['user_value'],
		'#prefix' => '<div class="dei-user-value dei-user-value-'.$mlid.' dei-user-value-'.$level.'">',
		 '#suffix' => '</div>',
      );
      $form[$mlid]['mlid'] = array(
        '#type' => 'hidden',
        '#value' => $item['mlid'],
      );
	  
		$form[$mlid]['#prefix'] = '<div class="dei-wrapper dei-wrapper-'.$level.'">';
		$form[$mlid]['#suffix'] = '</div>';
		$form[$mlid]['#tree'] = TRUE;
		$form[$mlid]['#description'] = t($item['description']);
    }

    if ($data['below']) {
      _dei_allocator_tree_form($data['below'],$form[$mlid],$level + 1 );
	  
	    $form[$mlid]['result_box'] = array(
          '#type' => 'fieldset',
		   '#prefix' => '<div class="dei-result-box dei-result-box-'.$mlid.' dei-result-box-'.$level.'">',
		   '#suffix' => '</div>',
      );
    }
  }

  return $form;
}

function _include_js_calc ($previous = 'Previous value',$current = 'Your value') {
  $js = '
    $(document).ready(function() {
    $(".dei-wrapper > .dei-user-value > div > input").val()
	 var sm = 10;
	/*$(".dei-wrapper-0 > .dei-wrapper").each(function() {
		sm += ($(this).find(".dei-old-value > div >input").val() - 0);
		$(this).parent().find(".dei-result-box").html(sm);
		
	});*/
$(".dei-wrapper-0 > .dei-result-box-0").html("<div>Vloni:1000 000</div><div>Muj navrh:800 00</div><img src= \"http://chart.apis.google.com/chart?cht=pc&chs=350x200&chd=t:70,30|30,30,10&chl=Kultura%28vloni%29|Sport%28vloni%29|Kultura%28ja%29|Sport%28ja%29|Uspora%28ja%29\" />");
	});
					/*if ($(this).find(".dei-wrapper").size() > 0) {*/
		/*}*/
		/*$(this).find(".dei-user-value > div > input").val()*/
  
    $("#dei-allocator-form > div > div")
	
	function  div_sum () {
	  
	}
 ';
 drupal_add_js ($js, 'inline');
}