<?php


namespace app\common;


class InterfaceStudent implements InterfacePerson
{
    private $vars = array();
    public function setVariable($name, $var)
    {
        $this->vars[$name] = $var;
    }
    public function getHtml($template)
    {
        foreach($this->vars as $name => $value) {
            $template = str_replace('{' . $name . '}', $value, $template);
        }
        return $template;
    }
    public function other($vars)
    {
        return $vars;
    }
}