<?php
	require("www2-funcs.php");
	require("www2-board.php");
	login_init();
	page_header("����������");
?>
<script type="text/javascript"><!--
var ta = new tabWriter(0,'main','����������',[['����',0,'center'],['���',0,0],['����',0,0]]);
<?php
		$i = 0;
		foreach ($section_names as $secname)
		{
			$i++;
?>
ta.r('<?php echo $i-1; ?>','<a href="bbsboa.php?group=<?php echo $i-1; ?>"><?php echo $secname[0]; ?></a>','<a href="bbsboa.php?group=<?php echo $i-1; ?>"><?php echo $secname[1]; ?></a>');
<?php
		}
?>
ta.t();
//-->
</script>
<?php
	bbs_boards_navigation_bar();
	page_footer();
?>

