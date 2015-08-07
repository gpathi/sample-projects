<?php

/**
 * @file
 *    Rollout drush command helper function.
 *
 */


define('DRUSH_EXECUTABLE_PATH', '/Applications/DevDesktop/drush/drush');

/**
 * Updated feature module.
 *
 * @param string $module_name
 *   Feature's machine name.
 * @param string $drupal_root
 *   Drupal root absolute directory path.
 *
 * @todo Remove $drupal_root parameter, Drush know the path.
 */
function hw_update_feature_module($module_name, $drupal_root) {
  // UPDATE the overridden feature.
  if (module_exists($module_name)) {
    $update_output = system(DRUSH_EXECUTABLE_PATH . ' features-update --yes ' . $module_name, $update_return);
    drush_print_r("Feature update output " . $revert_output);
    if ($update_return) {
      hw_drush_error_handler(dt('Could not update @module_name.', array('@module_name' => $module_name)), E_USER_ERROR);
    }
  }
  else {
    hw_drush_error_handler(dt('Could not find @module_name.', array('@module_name' => $module_name)), E_USER_ERROR);
  }
}

/**
 * Revert module.
 */
function hw_revert_feature_module($module_name, $root_dir) {
  // Revert the  feature.
  $return = chdir($root_dir);
  if ($return && module_exists($module_name)) {
    $revert_output = system(DRUSH_EXECUTABLE_PATH . ' features-revert --yes ' . $module_name, $revert_return);
    if ($revert_return) {
      hw_drush_error_handler(dt('Could not revert @module_name.', array('@module_name' => $module_name)), E_USER_ERROR);
    }
  }
}

/**
 * Commit the changes to site repository.
 */
function hw_checkin_feature_updates($site_repo_directory, $module_name, $branch_name = '7.x-1.x-stable', $commit_msg = '') {

  $working_dir = $site_repo_directory . '/' . $module_name;

  // Change directory to site specific directory.
  $return = chdir($working_dir);
  if ($return) {
    $dir = getcwd();
    if ($dir == $working_dir) {
      // Check git branch ; git commit ; git push.
      hw_drush_error_handler(dt('Committing code in Git'), E_USER_NOTICE);

      // Step-1 check the current git branch.
      // Note: 'git symbolic-ref --short HEAD' shows the current,
      // branch installed where as 'git branch' shows all local branches.
      exec('/usr/bin/git symbolic-ref --short HEAD', $output, $ret);
      if ($ret) {
        hw_drush_error_handler(dt('Current git branch is @output does not match the stable branch. Aborting...', array('@output' => $output)), E_USER_ERROR);
      }
      // We go ahead only if the branch is 7.x-1.x-stable ... correct ???
      if (($output) && ($output[0] == $branch_name)) {

        // Step-2 Commit the changes.
        hw_drush_error_handler(dt('Committing code into Git'), E_USER_NOTICE);
        $commit_msg = !($commit_msg) ? $commit_msg : 'Rolling out changes.';
        exec('/usr/bin/git commit -m "' . $commit_msg . '" .', $commit_output, $commit_return);

        // Commit is not successful, then flash the message n terminate the process.
        if ($commit_return) {
          hw_drush_error_handler(dt('Git commit failed..... need manual intervention.'), E_USER_ERROR);
        }

        // Step-3 Check if user has access to push things to remote.
        exec('/usr/bin/git push --dry-run origin ' . $branch_name . ' 2>&1', $push_output, $push_return);
        if ($push_return) {
          $push_output = array_filter($push_output);
          $error_message = implode(PHP_EOL, $push_output);
          hw_drush_error_handler($error_message, E_USER_ERROR);
        }

        // Step-4 Push changes to remote repo.
        hw_drush_error_handler(" Pushing code to remote \n", E_USER_NOTICE);
        exec('/usr/bin/git push origin ' . $branch_name, $push_output, $push_return);
        if ($push_return) {
          hw_drush_error_handler(dt('Git Push changes to remote repo failed.'), E_USER_ERROR);
        }
        else {
          hw_drush_error_handler(dt('Pushed changes to remote repo successfully !!!'), E_USER_NOTICE);
        }
      }
    }
    else {
      // Changed directory is not site specific directory.
      hw_drush_error_handler(dt('Could not change to site specific directory: @working_dir. Current director is @current_dir', array('@working_dir' => $working_dir, '@current_dir' => getcwd())), E_USER_ERROR);
    }
  }
  else {
    // Error ... could not change to site specific directory.
    hw_drush_error_handler(dt('Could not change to @working_dir from @current_dir', array('@working_dir' => $working_dir, '@current_dir' => getcwd())), E_USER_ERROR);
  }
}

/**
 * Error logger.
 */
function hw_drush_error_handler($message, $err_no = E_USER_NOTICE) {
  $error_message = '';
  $type = '';
  switch ($err_no) {
    case E_USER_ERROR:
      $error_message = " Error no: " . $err_no . " Error msg: " . $message;
      if (function_exists('hw_system_message')) {
        hw_system_message($error_message, 'error');
      }
      else {
        drush_log($error_message, 'error');
      }
      exit(1);

    case E_USER_WARNING:
      $error_message = "WARNING Error no: " . $err_no . " Error msg: " . $message;
      $type = 'warning';
      break;

    case E_USER_NOTICE:
      $error_message = "NOTICE Msg: " . $message;
      $type = 'notice';
      break;

    default:
      $error_message = "Unknown Error no: " . $err_no . " Error msg: " . $message;
      break;
  }

  if (function_exists('hw_system_message')) {
    hw_system_message($error_message, $type);
  }
  else {
    // @todo, remove or have alternate for drush_log to print
    // messages of type 'notice', 'message', 'info', if --verbose not set.
    $columns = drush_get_context('DRUSH_COLUMNS', 80);
    $width[1] = 11;
    $width[0] = ($columns - 11);

    $format = sprintf("%%-%ds%%%ds", $width[0], $width[1]);
    $error_message = wordwrap($error_message, $width[0]);
    $lines = explode("\n", $error_message);
    $type = sprintf("[%s]", $type);
    $lines[0] = sprintf($format, $lines[0], $type);
    $message = implode("\n", $lines);

    drush_print($message, 0, STDERR);
  }
  // error_log($error_message, 3, "/var/log/httpd/rollout.log");
  return TRUE;
}
