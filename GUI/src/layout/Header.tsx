import { Box, Flex, useColorModeValue, Stack } from "@chakra-ui/react";


const Header = () => {
  return (
    <Flex
      as="header"
      width="full"
      align="center"
      alignSelf="flex-start"
      justifyContent="center"
      gridGap={2}
    >
      <Box marginLeft="auto">
        <Box bg={useColorModeValue('gray.100', 'gray.900')} px={4}>
          <Flex h={16} alignItems={'center'} justifyContent={'space-between'}>
            <Box>Logo</Box>

            <Flex alignItems={'center'}>
              <Stack direction={'row'} spacing={7}>


              </Stack>
            </Flex>
          </Flex>
        </Box>
      </Box>
    </Flex>
  );
};

export default Header;
