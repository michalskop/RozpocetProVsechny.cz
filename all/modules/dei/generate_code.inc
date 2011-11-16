<?php
function dei_generate_code_form() {

    static $form = array('#tree' => TRUE);
    
    $form['description'] = array(
	  '#title' => 'Generování jedinečných kódů',
	  '#type' => 'fieldset',
	);
    $form['number'] = array(
      '#title' => 'Počet kódů',
      '#type' => 'textfield'
    );
    $form['nid'] = array(
      '#default_value' => arg(1),
	  '#type' => 'hidden',
    );
    $form['generate'] = array(
		'#type' => 'submit',
		'#value' => 'Generovat',
	);
	return $form;
}

function dei_generate_code_form_submit ($form, &$form_state) {
  //print_r($form_state);die();
  
  module_load_include('inc', 'node', 'node.pages');
  $node = array('type' => 'code');
  $form_state['values']['field_nid'][0]['value'] = $form_state['values']['nid'];
  $codes = '';
  if (is_numeric($form_state['values']['number'])) {
    for ($i=1; $i<=$form_state['values']['number']; $i++) {
      //generate code
      $codelenght = 10;
      $newcode = '';
	  while($newcode_length < $codelenght) {
	    $x=1;
	    $y=3;
	    $part = rand($x,$y);
	    if($part==1){$a=48;$b=57;}  // Numbers
	    if($part==2){$a=65;$b=90;}  // UpperCase
	    if($part==3){$a=97;$b=122;} // LowerCase
	    $code_part=chr(rand($a,$b));
	    $newcode_length = $newcode_length + 1;
	    $newcode = $newcode.$code_part;
	  }
	  $newcode_length = 0;
	  $node = new stdClass();
      $node->is_new=1;
      $node->title = "Code ".$form_state['values']['nid'] .'-'. $i;
      $node->body = "";
      $node->type = "code";
      $node->uid = $GLOBALS["user"]->uid;
      $node->teaser = "";
      $node->filter = 1;
      $node->status = 0;
      $node->comment = 2;
      $node->created = time();
      $node->changed = time();
      $node->field_nid[0]['value']=$form_state['values']['nid'];
      $node->field_code[0]['value']=$newcode;
      node_save($node);
      
      $codes .= $newcode . "<br/>";
    }
    // set message
    $message = "<b>Vygenerované jedinečné kódy</b>:<br/>" . $codes;
    drupal_set_message($message);
  }
}
?>
