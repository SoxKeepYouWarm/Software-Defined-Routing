#include "network_structures.h"
#include "gtest/gtest.h"
#include "stdio.h"

namespace {



	class Network_structures_test : public ::testing::Test {
	protected:

		Control_message* message;

		Network_structures_test() {
			message = 0;
			printf("running constructor\n");
		}

		virtual ~Network_structures_test() {
			printf("running destructor\n");
		}

		virtual void SetUp() {

			printf("running setup\n");
			const char msg[] = "Hello World!";

			message = new Control_message;
			message->payload = (void*)msg;
			message->header.payload_length = sizeof msg;

			printf("payload_length is: %d\n",
					message->header.payload_length);


		}

		virtual void TearDown() {

			printf("running teardown\n");
			delete message;

		}




	};

	TEST_F(Network_structures_test, first_test) {

		printf("running first_test\n");
		char* msg = (char*)message->payload;
		printf("payload: %.*s",
				message->header.payload_length, (char*)message->payload);

		}

	TEST_F(Network_structures_test, second_test) {

		printf("running second_test\n");
		char* msg = (char*)message->payload;
		printf("payload: %.*s",
				message->header.payload_length, (char*)message->payload);

	}

}


/*TEST(Network_structures_test, Simple_test) {

	const char msg[] = "Hello World!";

	Control_message message;
	message.payload = (void*)msg;

}*/


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
