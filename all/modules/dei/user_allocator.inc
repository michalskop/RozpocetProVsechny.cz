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
	// get nid
	if (!is_numeric(arg(1))) {
	  $arg = explode('/',drupal_lookup_path('source','allocator/'.arg(1)));
	} else {
	  $arg[1] = arg(1);
	}
	
	// get title, description, currency
	$query = "
	SELECT
	  n.title,
	  n.nid,
	  cta.field_dei_currency_value as currency,
	  cta.field_dei_description_value as description,
	  cta.field_dei_new_overall_value_value as new_overall_value
	FROM
	  node as n
	LEFT JOIN
	  content_type_allocator as cta
	ON n.vid = cta.vid
	WHERE cta.nid = %d
	";
	$q1 = db_query($query,array($arg[1]));
	while ($ai= db_fetch_array($q1)) {
		$allocator_info = $ai;
	}
	//get "view"
	// if a field is inside 2 or more content types, it has its own db table, otherwise it is in common db table for the field !!!!!
	$query = "
		SELECT 
			n.title as title,
			ctbc.vid as vid,
			ctbc.nid as mlid,
			ctbc.field_dei_allocator_nid as allocator_nid,
			ctbc.field_dei_category_description_value as description,
			ctbc.field_dei_category_weight_value as weight,
			ctbc.field_category_parent_nid as plid,
			ctbc.field_dei_user_value_value as user_value,
			ctbc.field_dei_previous_value_value as old_value
		FROM
			node as n
		LEFT JOIN
			content_type_budget_category as ctbc
		ON n.vid = ctbc.vid
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
	}
	
	if (isset($root_array)) {
		foreach ($root_array as $ra) {
		  $final_array[] = recursor($php_array, $ra);
		}
	} else {
		drupal_set_message(t('No root budget category. Choose/add at least one. Or you do not have rights to access them.'));
	}
	//drupal_set_message('<pre>' .  print_r($final_array,1). '</pre>');
	   if (isset($final_array)) {
	   
		$form = _dei_allocator_tree_form($final_array,$allocator_info);
		//drupal_set_message('<pre>' .  print_r($form,1). '</pre>');
		array_unshift($form, array (
			'#title' => $allocator_info['title'],
			'#description' => t($allocator_info['description']),
			'#type' => 'fieldset',
		));
		  $form['infotext'] = array(
				'#type' => 'fieldset',
				'#prefix' => '<div id="infotext-wrapper">', // This is our wrapper div.
				'#suffix' => '</div>',
			);
		  $form['nid'] = array(
				'#type' => 'hidden',
				'#value' => $allocator_info['nid'],
			);
		/*module_load_include('inc', 'captcha');;*/
		/*$form['my_captcha_element'] = array(
		  '#type' => 'captcha',
		);*/
		  $form['save'] = array(
				'#type' => 'submit',
				'#value' => t('Continue'),
			);
		}
	  //include custom js
	  _include_js_calc($allocator_info['currency'],$allocator_info['new_overall_value']);
	  
	 return $form;

}
/**
*
*/
function _dei_allocator_tree_form($tree,$allocator_info = array(),&$form = array('#tree' => TRUE),$level = 0) {
//drupal_set_message('<pre>' .  print_r($tree,1). '</pre>');
  //static $form = array('#tree' => TRUE);

  foreach ($tree as $data) {
	$currency = $allocator_info['currency'];
    $item = $data['link'];
	
	
    // Don't show callbacks; these have $item['hidden'] < 0.
    if ($item && $item['hidden'] >= 0) {
      $mlid = 'mlid:'. $item['mlid'];
      $form[$mlid]['#item'] = $item;
      $form[$mlid]['#attributes'] = $item['hidden'] ? array('class' => 'menu-disabled') : array('class' => 'menu-enabled');
      $form[$mlid]['title']['#value'] = $item['title'] . ($item['hidden'] ? ' ('. t('disabled') .')' : '');
	  $form[$mlid]['title']['#prefix'] = '<div class="dei-title dei-title-'.$level.'">';
	  $form[$mlid]['title']['#suffix'] = '</div>';
      //$form[$mlid]['#description'] = $item['description'];
	  $form[$mlid]['description'] = array(
        '#type' => 'fieldset',
		'#description' => $item['description'],
      );

	  $form[$mlid]['user_value'] = array(
        '#type' => 'textfield',
        '#default_value' => $item['user_value'],
		//'#value' => $item['user_value'],
		'#prefix' => '<div class="dei-user-value dei-user-value-'.$mlid.' dei-user-value-'.$level.'">' . 'Váš návrh:',
		 '#suffix' => $currency . '</div>',
		 '#size' => 10,
      );
      $form[$mlid]['old_value'] = array(
        '#type' => 'textfield',
        '#default_value' => $item['old_value'],
		'#value' => $item['old_value'],
		'#disabled' => TRUE,
		'#prefix' => '<div class="dei-old-value dei-old-value-'.$mlid.' dei-old-value-'.$level.'">' . 'Vloni:',
		 '#suffix' => $currency . '</div>',
		 '#size' => 10,
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
      _dei_allocator_tree_form($data['below'],$allocator_info,$form[$mlid],$level + 1 );
	  
	    $form[$mlid]['result_box'] = array(
          '#type' => 'fieldset',
		   '#prefix' => '<div class="dei-result-box dei-result-box-'.$mlid.' dei-result-box-'.$level.'">',
		   '#suffix' => '</div>',
      );
    }
  }

  return $form;
}

function _include_js_calc ($currency = '', $new_overall_value = 1, $previous = 'Previous value',$current = 'Your value') {
  $js = str_replace('_NEW_OVERALL_VALUE',$new_overall_value,str_replace('_CURRENCY',$currency,file_get_contents(drupal_get_path('module', 'dei') . '/dei_allocator_inc.js')));
 drupal_add_js ($js, 'inline');
 drupal_add_js(drupal_get_path('module', 'dei') . '/../../js/jquery.numberformatter-1.2.js');
 
}

function dei_allocator_form_submit ($form, &$form_state) {
  
  $form_state['redirect'] = $_GET['q'] . '/submit';
  $_SESSION['user_values']= $form_state['values'];
  //print_r($form_state);die();
}