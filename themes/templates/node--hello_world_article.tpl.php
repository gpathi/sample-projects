<?php
 
 drupal_add_css(path_to_theme() . '/css/hello_world_article.css', array ('group' => CSS_THEME));


?>

<div id='parent_div_1'>
    <div class='snippet'>Content starts here! </div>
    <div class ='child_div_1'> 
      <?php 
          //  print render($content);
            print "this is test content";
      ?>
    </div>
    <div class='child_div_2'>
        <?php
         
         $block = module_invoke('helloworld', 'block_view', 'helloworld_blk');
         print render($block['subject']);
         print render($block['content']);
         
         ?>
    </div>
</div>