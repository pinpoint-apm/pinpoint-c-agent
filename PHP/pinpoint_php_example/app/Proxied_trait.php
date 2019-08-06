<?php


namespace app;


trait Proxied_trait
{
    use TestTrait{
        TestTrait::getReturnDescription as Proxied_trait_getReturnDescription;
    }

    public function getReturnDescription()
    {
        $this->Proxied_trait_getReturnDescription();
    }
}
