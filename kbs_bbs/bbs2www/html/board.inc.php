<?php
function bbs_boards_navigation_bar()
{
?>
<p align="center">
[<a href="/mainpage.html">��ҳ����</a>]
[<a href="/bbssec.php">����������</a>]
[<a href="/bbsnewbrd.php">�¿�������</a>]
[<a href="/bbsrecbrd.php">�Ƽ�������</a>]
[<a href="/bbsbrdran.php">��������������</a>]
[<a href="/cgi-bin/bbs/bbs0an">����������</a>]
[<a href="javascript:history.go(-1)">���ٷ���</a>]
<br />
</p>
<?php	
}

function undo_html_format($str)
{
	$str = preg_replace("/&gt;/i", ">", $str);
	$str = preg_replace("/&lt;/i", "<", $str);
	$str = preg_replace("/&quot;/i", "\"", $str);
	$str = preg_replace("/&amp;/i", "&", $str);
	return $str;
}

# iterate through an array of nodes
# looking for a text node
# return its content
function get_content($parent)
{
    $nodes = $parent->child_nodes();
    while($node = array_shift($nodes))
        if ($node->node_type() == XML_TEXT_NODE)
            return $node->node_value();
    return "";
}

# get the content of a particular node
function find_content($parent,$name)
{
    $nodes = $parent->child_nodes();
    while($node = array_shift($nodes))
        if ($node->node_name() == $name)
            return undo_html_format(urldecode(get_content($node)));
    return "";
}

?>