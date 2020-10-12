<?php

/**
 * @file
 * Code for Rolling Out Google Scholar and Infinite Loop Changes 
 */

define('DRUPAL_ROOT', getcwd());

include_once 'hw_rollout.helper.inc';

$jcode = drush_get_option('jcode');
$jnldir = drush_get_option('jnldir');
$sitedir = drush_get_option('sitedir');

if (isset($jcode) && isset($jnldir) && isset($sitedir) && file_exists($jnldir)) {

   drush_print("Starting Google changes for Site: " . $jcode . "  with site specific module path at " . $jnldir);
   hw_google_rollout($jcode,$jnldir,$sitedir);
}
else {
  hw_drush_error_handler("Usage: drush --uri=<site url> scr <php script> --jcode=<jcode> --jnldir=<path of site dir> --sitedir=<site dir name>" , E_USER_NOTICE);
  hw_drush_error_handler("JCode and Site specific directory path parameters have not been passed. Exiting. " , E_USER_ERROR);
}


/**
*
*  Make the configuration changes needed for Google Scholar and Infinite loop changes
*  hw_google_rollout
*
*/

function hw_google_rollout ($jcode,$jnldir,$sitedir) {
	   
$modulename = "jnl_" . $jcode . "_entity_pages";
$minipanel =  "jnl_" . $jcode . "_tab_art";
$pdftab = "jnl_". $jcode . "_tab_pdf";
$stablebranch = "7.x-1.x-stable";
$commitmsg = "Committing Google Scholar and Infinite Loop Changes ";


$rootdir = $jnldir. "/" . "drupal-webroot" ;

if (!file_exists($rootdir)) {

  hw_drush_error_handler("Site specific directory does not exist. Exit. " , E_USER_ERROR);
}

ctools_include('plugins');
ctools_include('page', 'page_manager', 'plugins/tasks');
ctools_include('page_manager.admin', 'page_manager', '');
ctools_include('export');
$minis = ctools_export_load_object('panels_mini');
 
 // Google Scholar Changes

 // Loop through all the mini panels
      foreach ($minis as $mini) {
        
        
        // Update the configuration of article tab mini panel
	     if ($mini->name == $minipanel){
                     
 				
			   foreach ($mini->display->content as $contentObj)  {
                                    
					 if ($contentObj->type == 'hw_markup') {
						
					    $contentObj->configuration['process']['hw_google_scholar'] = 'hw_google_scholar';
					    hw_drush_error_handler("Google Scholar configuration changes completed for site " . $jcode . "\n"  , E_USER_NOTICE);
					    break;
					 }
					
			   }
        	   // Save panel diplay with updated configuration
               $savedDisplay = panels_save_display($mini->display);
               $mini->did = $savedDisplay->did;
               $mini->display = $savedDisplay;
               $update = (isset($mini->pid) && $mini->pid != 'new') ? array('pid') : array();
               drupal_write_record('panels_mini', $mini, $update);
		      
		 
	     } 
               
	  }  

     // Google Infinity Loop Changes.

      // Load all pages
      $tasks = page_manager_get_tasks_by_type('page');
      $page_types = array();

      foreach ($tasks as $task) {
            // Get al the page handlers
            if ($pages = page_manager_load_task_handlers($task)) {
              $page_types[] = $pages;
            }
      }

      // Load all display objects
      foreach ($page_types as &$pages) {
		 
            foreach ($pages as &$page) {
                     if ($page && (substr($page->name,0,9) == 'node_view')) {
                      
                           $display = $page->conf['display'];
                        
                            
                           foreach ( $display->content as $contentobj ) {

                                 if ($contentobj->type == "hw_panel_tabs") { // hw_panel_tabs

								   $contentobj->configuration['mini_panels'][$pdftab]['url_id'] = "full.pdf+html";      
                                   
                                 }
								
                           }
						  
						   $savedisplay = panels_save_display($display);
                           $page->conf['display'] =  $savedisplay;
						   page_manager_save_task_handler($page);
						   hw_drush_error_handler("Google Infinite Loop changes completed for site " . $jcode . "\n"  , E_USER_NOTICE);
                      
                     }
            }
      }

     hw_update_feature_module($modulename, $rootdir);
     hw_clear_drupal_cache();
   //  hw_checkin_feature_updates($jnldir . "/" . $sitedir . "/modules", $modulename, $stablebranch, $commitmsg);

}

/**
*
* Clear drupal cache through Drush 
*
* hw_clear_drupal_cache
*/


function hw_clear_drupal_cache(){
	   // Clear drupal cache
       exec("drush cc all",$cacheoutput,$cachereturn);
	 
}
