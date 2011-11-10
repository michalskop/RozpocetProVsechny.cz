<?php


function dei_allocator_sub_form() {

$values = $_SESSION['user_values'];
  //print_r($values);

if ($values['form_id'] != 'dei_allocator_form') {
  drupal_access_denied();
  exit();
} else {
	static $form = array('#tree' => TRUE);
	$form['description'] = array(
	  '#title' => 'Řekněte nám něco o sobě',
	  '#description' => 'Řekněte nám něco o sobě, pomůžete sami sobě',
	  '#type' => 'fieldset',
	);
	$form['first_name'] = array (
	  '#title' => 'Jméno',
	  '#type' => 'textfield',
	);
	$form['last_name'] = array (
	  '#title' => 'Příjmení',
	  '#type' => 'textfield',
	);
	$form['sex'] = array (
	  '#title' => 'Pohlaví',
	  '#type' => 'radios',
	  '#options' => array(t('Žena'), t('Muž')),
	);
	$form['dei_address'] = array (
	  '#title' => 'Trvalé bydliště',
	  '#type' => 'textfield',
	);
	$form['dei_birth'] = array (
	  '#title' => 'Rok narození',
	  '#type' => 'textfield',
	);
	$form['user_comment'] = array (
	  '#title' => 'Komentář',
	  '#type' => 'textarea',
	);
	require_once ('array2xml.php');
	$form['xml'] = array (
	  '#value' => serialize($values),
	  '#type' => 'hidden',
	);
	$form['nid_allocator'] = array(
	  '#default_value' => $values['nid'],
	  '#type' => 'hidden',
	);
	$form['ip_hash'] = array(
	  '#default_value' => md5($_SERVER['REMOTE_ADDR']),
	  '#type' => 'hidden',
	 );
	$form['save'] = array(
		'#type' => 'submit',
		'#value' => t('Save'),
	);
  return $form;
  }
}

function dei_allocator_sub_form_submit ($form, &$form_state) {
  foreach ($form_state['values'] as $key => $row) {
    $form_state['values']['field_' . $key][0]['value'] = $form_state['values'][$key];
  }
  //print_r($form_state);
  
  module_load_include('inc', 'node', 'node.pages');
  $node = array('type' => 'user_allocator_submit');
  /*print_r($form_state);
  die();*/
  $form_state['values']['title'] = 'Submission ' . date('Y-m-d H:m:s');
  drupal_execute('user_allocator_submit_node_form', $form_state, (object)$node);
  $form_state['redirect'] = arg(0) . '/' . arg(1) . '/result';
}