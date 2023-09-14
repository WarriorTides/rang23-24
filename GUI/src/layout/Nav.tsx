// import React from 'react';
import { Box, Flex, Link, Text } from '@chakra-ui/react';

function NavBar() {

  return (
    <Flex
      as="nav"
      align="center"
      justify="space-between"
      wrap="wrap"
      padding="1rem"
      bg="purple.800"
      color="white"
    >
      <Flex align="center" mr={5}>
        <svg fill='white' width="54" height="54" viewBox="0 0 54 54" xmlns="http://www.w3.org/2000/svg"><path d="M13.5 22.1c1.8-7.2 6.3-10.8 13.5-10.8 10.8 0 12.15 8.1 17.55 9.45 3.6.9 6.75-.45 9.45-4.05-1.8 7.2-6.3 10.8-13.5 10.8-10.8 0-12.15-8.1-17.55-9.45-3.6-.9-6.75.45-9.45 4.05zM0 38.3c1.8-7.2 6.3-10.8 13.5-10.8 10.8 0 12.15 8.1 17.55 9.45 3.6.9 6.75-.45 9.45-4.05-1.8 7.2-6.3 10.8-13.5 10.8-10.8 0-12.15-8.1-17.55-9.45-3.6-.9-6.75.45-9.45 4.05z" /></svg>

        <Text pl={"3"} fontSize="lg" fontWeight="bold">
          WarriorWaves
        </Text>
      </Flex>

      <Box
        display="flex"
        width="auto"
        alignItems="center"
        flexGrow={1}
      >
        <Link href="/" m={2}>Home</Link>
        <Link href="/about" m={2}>About</Link>
        <Link href="/blog" m={2}>Blog</Link>
        <Link href="/contact" m={2}>Contact</Link>
      </Box>
    </Flex>
  );
}

export default NavBar;
