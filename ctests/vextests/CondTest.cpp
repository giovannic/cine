void *MonitorTest (void *arg){

	int R1 = 1;
	int R2 = 2;
	int R3 = 3;
	int R4 = 4;
	int R5 = 5;
	int R6 = 6;
	int R7 = 7;
	
	bool waiting;

  id = (int)arg;
	
	private static Thread mainThread = null;
	public void run() {
		try {
			switch (id) {
				case R1:
				case R2:
					Thread.sleep(2000);
					synchronized(MonitorTest.class) {
						MonitorTest.class.notifyAll();
						MonitorTest.waiting = true;
						System.out.println("after notifying and setting at " + System.nanoTime());
					}
					System.out.println("RESULT: main should be interrupted and finish after 2sec");
					break;
				case R3:
					Thread.sleep(1500);
					synchronized(MonitorTest.class) {
						MonitorTest.waiting = true;
					}
					System.out.println("RESULT: main should print two times main awake and finish after 2sec");
					break;
				case R4:
					
					synchronized(MonitorTest.class) {
						MonitorTest.class.wait(3500);
						MonitorTest.waiting = true;
					}
					System.out.println("RESULT: main should print four times main awake and finish after 4sec");
					break;
				case R5:
					
					synchronized(MonitorTest.class) {
						MonitorTest.class.wait(3500);
						MonitorTest.waiting = true;
						MonitorTest.class.notifyAll();
					}
					System.out.println("RESULT: main should print four times main awake and finish after 3.5sec");
					break;				
				case R6:
					
					synchronized(MonitorTest.class) {
						MonitorTest.class.wait(3500);
						mainThread.interrupt();
					}
					System.out.println("RESULT: main should print three times main awake and finish after 3.5sec");
					break;		
				case R7:

					synchronized(this) {
						wait(6500);
						MonitorTest.waiting = true;
						notifyAll();
					}
					System.out.println("RESULT: main should print 6 times main awake and finish after 6.5sec");
					break;					
				default:
			}
		} catch (InterruptedException x) {
			
		}
			
	}
	
	
	private synchronized void doWait() throws InterruptedException {
		wait(1000);	
	}

	private static synchronized void checkAndWait() throws InterruptedException { 
		while (!MonitorTest.waiting) {
			MonitorTest.class.wait(1000);
			System.out.println("second passed");
		}
	}
	
	
	
	private synchronized void checkWaitInst() throws InterruptedException {
		while (!MonitorTest.waiting) {
			doWait();
			System.out.println("second passed");
		}
	}
	
	public static void programLegend() {
		System.out.println("Syntax error: java vtf_tests.MonitorTest <0-7>");
		System.exit(-1);	
	}
	
	public static void main(String[] args) {
		if (args.length != 1) {
			programLegend();
		}
		
		int testId = Integer.parseInt(args[0]);
		long start;
		
		waiting = false;
		MonitorTest si = new MonitorTest(testId);
		mainThread = Thread.currentThread();
		
		start= System.nanoTime();
		Thread t = new Thread(si, "interruptingThread");
		t.start();

		try {
			switch (testId) {
				case R1:
					synchronized(MonitorTest.class) {
						do {
							System.out.println("before waiting");
							MonitorTest.class.wait();
							System.out.println("after waiting");
						} while(!MonitorTest.waiting);
					}
					break;
				case R2:
					synchronized(MonitorTest.class) {
						do {
							MonitorTest.class.wait(50000);
						} while(!MonitorTest.waiting);	
					}
					break;
				case R3:
				case R4:
				case R5:
				case R6:
					synchronized(MonitorTest.class) {
						while(!MonitorTest.waiting) {
							MonitorTest.checkAndWait();
						}
					}
					break;
				case R7:
					
					synchronized(si) {
						while(!MonitorTest.waiting) {
							si.checkWaitInst();
						}
					}
					break;
				default:
			}
		} catch (InterruptedException x) {
			System.out.println("main interrupted!");
		}

		
		System.out.println((System.nanoTime() - start)/1e9);
	}
	
}
