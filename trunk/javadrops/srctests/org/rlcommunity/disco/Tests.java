package org.rlcommunity.disco;

import org.junit.runner.RunWith;
import org.junit.runners.Suite;
import org.rlcommunity.disco.datagroup.DiscoSocketTest;
import org.rlcommunity.disco.datagroup.DropTest;
import org.rlcommunity.disco.utils.SignalTest;

@RunWith(Suite.class)
@Suite.SuiteClasses( { SignalTest.class, DropTest.class, DiscoSocketTest.class })
public class Tests {
}