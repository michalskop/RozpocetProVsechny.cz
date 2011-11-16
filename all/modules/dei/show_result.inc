<?php


function dei_show_result_form() {

	// get nid
	if (!is_numeric(arg(1))) {
	  $arg = explode('/',drupal_lookup_path('source','allocator/'.arg(1)));
	} else {
	  $arg[1] = arg(1);
	}
	
	// get array
	$query = "
SELECT
		  ctuas.*,
		  ctuas.field_xml_value as xml,
		  ctc.field_code_value
		FROM
		  content_type_user_allocator_submit as ctuas
		LEFT JOIN
		  content_type_code as ctc
		ON ctuas.field_key_code_value= ctc.field_code_value AND ctuas.field_nid_allocator_value=ctc.field_nid_value
		WHERE ctuas.field_nid_allocator_value = 1
	    ORDER BY ctuas.nid
	";
	$q = db_query($query,array($arg[1]));
	//$ip_restriction = ($_POST[3] == 1);
	$code_restriction = ($_POST[4] == 1);
	if (is_numeric($_POST[2])) $lo_limit = $_POST[2]; else $lo_limit = 'Not set';
	if (is_numeric($_POST[1])) $hi_limit = $_POST[1]; else $hi_limit = 'Not set';
	
	while ($r = db_fetch_array($q)) {
	  if ($code_restriction) {
	    //$php_ar[$r['field_ip_hash_value']] = _strip_unuseful(unserialize($r['xml']));
		if (is_array($pom = _strip_unuseful(unserialize($r['xml'])))) {
		  $row_ar = array();
	      _flat_it($pom,$row_ar);
		}
	   if((!empty($row_ar)) and _within_limits(reset($row_ar),$lo_limit,$hi_limit)) {
	     $php_ar[] = $row_ar;
		 foreach ($row_ar as $key => $item) {
		   if ($r['field_code_value'] != '')
		     $result_ar[$key][$r['field_code_value']] = $item;
		 }
		}
	  } else {
	    if (is_array($pom = _strip_unuseful(unserialize($r['xml'])))) {
		  $row_ar = array();
	      _flat_it($pom,$row_ar);
		}
	   if((!empty($row_ar)) and _within_limits(reset($row_ar),$lo_limit,$hi_limit)) {
	     $php_ar[] = $row_ar;
		 foreach ($row_ar as $key => $item) {
		     $result_ar[$key][] = $item;
		 }
	   }
	  }
	}
	//drupal_set_message('<pre>' . print_r($_POST,1) . '</pre>');
	
	//form
	module_load_include('php','dei','q_chart_0.2.2');
	return dei_allocator2_form(&$form_state,$result_ar);
}



/**
*
*/
/*function dei_show_result_form_validate ($form, &$form_state) {
  $_SESSION['user_values']['ip_restricted'] = $form_state['values'][3];
  $_SESSION['user_values']['lo_limit'] = $form_state['values'][2];
  $_SESSION['user_values']['hi_limit'] = $form_state['values'][1];
  print_r($_SESSION);
  return true;
}*/


/**
* compare the limits
*/
function _within_limits ($number, $lo_limit = 'Not set', $hi_limit = 'Not set') {
  if (($lo_limit == 'Not set') or ($lo_limit <= $number)) $lo = TRUE; else $lo = FALSE;
  if (($hi_limit == 'Not set') or ($hi_limit >= $number)) $hi = TRUE; else $hi = FALSE;
  return ($lo AND $hi);
}


/**
*  strip fields, whch are not mlid
*/
function _strip_unuseful ($array = array()) {
  if (!empty($array)) {
    foreach ($array as $key => $row) {
		if (!(strpos($key,'mlid:') === false)) {
		  $php_a[$key] = $row;
		}
	}
  }
  return $php_a;
}




/**
* * flat the structure
*/
function _flat_it($array = array(), &$out = array(), $key_part='mlid:') {
  if (!empty($array)) {
    foreach ($array as $key => $row) {
	  if (!(strpos($key,'mlid:') === false)) {
		$out[$key] = $row['user_value'];
		if (!(empty($row)) and is_array($row)) {
			foreach ($row as $key_item => $item) {
				if (!(strpos($key_item,'mlid:') === false)) {
					_flat_it($row,$out);
				}
			}
		}
	  }
	}
  }
  return $out;
}



/**
* make histogram
*/
function _make_histogram ($array = array(), $old_value, $cat = 5 ) { //chart hardcoded to $cat = 5!!
  if (!empty($array)) {
	  $out['max'] = max($old_value,max($array));
	  $out['min'] = min($old_value,min($array));
	  $out['median'] = median($array);
	  $out['mean'] = array_sum($array)/count($array);
	  $int = ($out['max'] - $out['min'])/$cat;
	  //set 0s
	  for ($i = 0; $i < $cat; $i++) {
	    $out['histo'][$i] = 0;
	  }
	  //make counts
	  foreach ($array as $row) {
	    if ($int != 0) {
	      $pom = ceil(($row - $out['min']) / $int);
		} else {
		  $pom = 0;
		}
		//correction for limit value
		if ($pom == $cat) $pom = $cat - 1;
	    $out['histo'][$pom]++;
	  }
  }
  return ($out);
}


function dei_allocator2_form(&$form_state, $result_ar) {
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
	  cta.field_dei_description_value as description
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
	  $mlid = $r['mlid'];
	  $php_array[$mlid] = $r;
	  $php_array[$mlid]['has_children'] = 0;
	  if (!($r['plid'] > 0)) {
		$root_array[] =& $php_array[$mlid];
		$php_array[$mlid]['plid'] = 0;
	  }
	  //histogram categories
	  $php_array[$mlid]['result'] = _make_histogram($result_ar['mlid:'.$mlid], $r['old_value']);
	}

	//print_r($php_array);
	foreach ($php_array as $pa) {
	  if ($pa['plid'] > 0) {
	    $php_array[$pa['plid']]['children'][$pa['mlid']] = $pa['mlid'];
		//echo $pa['plid'] . '<br/>';
	  }
	}
		//drupal_set_message('<pre>' . print_r($php_array,1) . '</pre>');
	if (isset($root_array)) {
		foreach ($root_array as $ra) {
		  $final_array[] = recursor($php_array, $ra);
		}
	} else {
		drupal_set_message(t('No root budget category. Choose/add at least one. Or you do not have rights to access them.'));
	}
	//drupal_set_message('<pre>' .  print_r($final_array,1). '</pre>');
	   if (isset($final_array)) {
	   
		$form = _dei_allocator_tree2_form($final_array,$allocator_info);
		//drupal_set_message('<pre>' .  print_r($form,1). '</pre>');
			array_unshift($form, array(
				'#type' => 'submit',
				'#value' => t('Recalculate'),
				'#prefix' => '<div id="recalculate-wrapper">', // This is our wrapper div.
				'#suffix' => '</div>',
			));
			//$pom = $_POST[3];
			array_unshift($form, array(
				'#type' => 'checkbox',
				'#default_value' => $_SESSION['user_values']['code_restricted'] ,
				'#title' => 'Jen s vyplněným jednoznačným kódem',
				'#prefix' => '<div id="key-code-wrapper">', // This is our wrapper div.
				'#suffix' => '</div>',
			));
			array_unshift($form, array(
				'#type' => 'hidden',//'checkbox',
				'#default_value' => $_SESSION['user_values']['ip_restricted'] ,
				'#title' => t('Only 1 allocator per each IP'),
				'#prefix' => '<div id="one-ip-checkbox-wrapper">', // This is our wrapper div.
				'#suffix' => '</div>',
			));
	   	   array_unshift($form, array(
				'#type' => 'textfield',
				'#title' => t('Low limit for whole allocator'),
				'#prefix' => '<div id="lo-limit-wrapper">', // This is our wrapper div.
				'#suffix' => '</div>',
			));
	   	   array_unshift($form, array(
				'#type' => 'textfield',
				'#title' => t('High limit for whole allocator'),
				'#prefix' => '<div id="lo-limit-wrapper">', // This is our wrapper div.
				'#suffix' => '</div>',
			));
		array_unshift($form, array (
			'#title' => $allocator_info['title'],
			'#type' => 'fieldset',
		));
	   }
	$form['#redirect'] = FALSE;
	return $form;

}
/**
*
*/
function _dei_allocator_tree2_form($tree,$allocator_info = array(),&$form = array('#tree' => TRUE),$level = 0) {
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
	  if (!empty($item['result']['histo'])) {
		  $form[$mlid]['chart'] = array (
			'#type' => 'fieldset',
			'#prefix' =>  _chart($item),
		  );
	  }
		$form[$mlid]['#prefix'] = '<div class="dei-wrapper dei-wrapper-'.$level.'">';
		$form[$mlid]['#suffix'] = '</div>';
		$form[$mlid]['#tree'] = TRUE;
		$form[$mlid]['#description'] = t($item['description']);
    }

    if ($data['below']) {
      _dei_allocator_tree2_form($data['below'],$allocator_info,$form[$mlid],$level + 1 );
	  
    }
  }

  return $form;
}

/**
* median
*/

function median()
{
    $args = func_get_args();
    switch(func_num_args())
    {
        case 0:
            //trigger_error('median() requires at least one parameter',E_USER_WARNING);
            return false;
            break;
        case 1:
            $args = array_pop($args);
            // fallthrough
        default:
            if(!is_array($args)) {
                //trigger_error('median() requires a list of numbers to operate on or an array of numbers',E_USER_NOTICE);
                return false;
            }
            sort($args);         
            $n = count($args);
            $h = intval($n / 2);
            if($n % 2 == 0) {
                $median = ($args[$h] + $args[$h-1]) / 2;
            } else {
                $median = $args[$h];
            }
            break;
    }  
    return $median;
}

function _chart($item) {
	$max = round(max($item['result']['histo']));
	if ($item['result']['min'] != $item['result']['max']) {
		$median = round(($item['result']['median'] - $item['result']['min']) / ($item['result']['max'] - $item['result']['min']) * 80+10);
		$old_value = round(($item['old_value'] - $item['result']['min']) / ($item['result']['max'] - $item['result']['min']) * 80+10);
	} else {
		$median = 0;
		$old_value = 0;
	}
	$in = array(
	  'type' => 'bvs',
	  'data' => $item['result']['histo'],
	  'encoding' => 's',
	  'size' => array(300,150),
	  'axis' => array('x','y','x','x','y','x'),
	  'axis_label' => array(
			0 => array($item['result']['min'],$item['result']['max']),
			1 => array(0,$max),
			2 => array('střed:'.$item['result']['median']),
			3 => array('minule:'.$item['old_value']),
			4 => array('počet lidí'),
			5 => array('Kč'),
		),
	  'axis_label_position' => array(
			array(10,90),
			array(10,90),
			array($median),
			array($old_value),
			array(50),
			array(50),
		),
		'axis_style' => array(2 => 'N*z0*,FF0000,11,0,t,FF0000', 3 => 'N*z0*,008000,11,0,t,008000'),
		'axis_tick_length' => array(2 => array(-100),3 => array(-100) ),
	);
  return q_common_chart($in);
}
