<?php

/**
*@file
* Sample Application Administration
*/
function sample_application_form($form, $form_state) {
  $form = array();

  $header = array(
    'title' => t('Flagged Content'),
    'name' => t('Username'),
    'entity_id' => t('Node ID'),
    'flagging_id' => t('Flag ID'),
     'status' => t('Status'),

  );
  // name and alias-- sa -- f--
  $sql = db_select('sample_application', 'sa')->extend('PagerDefault');
  $sql->join('flagging', 'f', 'f.flagging_id = sa.flagging_id');
  $sql->join('node', 'n', 'f.entity_id = n.nid');
  $sql->join('users', 'u', 'f.uid = u.uid');
  $sql
    ->fields('sa', array('status'))
    ->fields('f', array('entity_id', 'timestamp', 'flagging_id'))
    ->fields('n', array('title'))
    ->fields('u', array('name', 'uid'))
    ->orderBy('timestamp', 'DESC')
    ->limit(25);
  $results = $sql->execute();

  $rows = array();

  foreach ($results as $result) {
    switch($result->status){
      case 0:
        $status ='Pending';
        break;
      case 1:
        $status = 'Approved';
        break;
      case 2:
        $status = 'Denied';
        break;
      }

      $rows[] = array(
          'title' => l($result->title, 'node/'.$result->entity_id),
          'name' => l($result->name, 'user/'.$result->uid),
          'entity_id' => $result->entity_id,
          'uid' => $result->uid,
          'flagging_id' => $result->flagging_id,
          'status' => $status,
          '#attributes' => array('class' => array('application_row')),

      );


  }

 $form['sample_application_status']['approvedeny'] = array(
     '#type' => 'select',
     '#title' => 'Actions',
     '#options' => array(
         1 => t('Approve'),
         2 => t('Deny'),

     ),
  );
 $form['sample_application_table'] = array(
     '#type' => 'tableselect',
     '#header' => $header,
     '#options' => $rows,
     '#empty' => t('No applications found.'),
     '#attributes' => array('class' => array('applications')),
 );
 //pager
 $form['pager'] = array('#markup' => theme('pager'));

 $form['submit'] = array(
     '#type' => 'submit',
     '#value' => t('Submit'),
 );
 return $form;
}
function sample_application_form_submit($form, $form_state) {
  //dsm($form);
  //dsm($form_state);
  $action = $form_state['values']['approvedeny'];
  $values = array();
  foreach ($form_state['values']['sample_application_table'] as $key=> $value) {
    if (is_string($value) == TRUE) {
      $values[] = $form_state['complete form']['sample_application_table']['#options'][$value];

    }
  }
    sample_application_administration($action, $values);

}
function sample_application_form_validate($form, $form_state){
  dsm($form);
  dsm($form_state);
  if ($form_state['values']['approvedeny'] != 1 && $form_state['values']['approvedeny'] != 2) {
    form_set_error('approvedeny', "You have provided an incorrect value");
  }

}

  function sample_application_administration($action, $values) {
    $status = ($action == 1) ? 'Approved' : 'Denied';
    foreach ($values as $key => $value) {
      $num_updated = db_update('sample_application')
        ->fields(array('status' => $action))
        ->condition('flagging_id', $value['flagging_id'])
        ->execute();
      watchdog('sample_application',t("!name's application for !node was !status",
          array('!name' => $value['name'], '!node' => $value['title'], '!status' => $status)));

    }

  }




