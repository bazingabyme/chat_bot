from openai import OpenAI
from dotenv import load_dotenv
import os
from fastapi import FastAPI
from pydantic import BaseModel

# MODEL_NAME = "gpt-4o-mini"
MODEL_NAME = "gpt-4.1-mini"

META_DATA = {
  "document_title": "Operating Instructions: Blister Machine N 921",
  "document_id": "BA 921-10201435",
  "manufacturer": "Romaco Pharmatechnik GmbH",
  "machine_model": "Noack N 921",
  "year_of_construction": 2010,
  "document_type": "Technical Manual",
  "customer": "Dr. Reddy's Laboratories Ltd.",
  "machine_use": "Deep-draw formable films (PVC, Alu, PP), fill, and seal with lidding foil to produce blister packs",
  "intended_users": "Trained technicians and operators in pharmaceutical/industrial packaging",
  "language": "English",
  "format": "PDF",
  "domain": "Industrial Automation / Packaging Machinery",

  "topic_categories": {
    "technical_specifications": [
      "Dimensions",
      "Power requirements",
      "Air pressure",
      "Cooling requirements",
      "Forming depth",
      "Film roll specifications"
    ],
    "safety": [
      "Personal protective equipment",
      "Hazard symbols",
      "Machine hazards",
      "Electrical safety",
      "EC directives"
    ],
    "setup_and_installation": [
      "Site requirements",
      "Machine transport",
      "Electrical connection",
      "Compressed air and cooling setup",
      "Leveling"
    ],
    "machine_components": [
      "Heating station",
      "Forming station",
      "Feeding system",
      "Sealing station",
      "Coding unit",
      "Perforation and die-cutting",
      "Discharging system"
    ],
    "operation": [
      "Startup procedure",
      "Manual and automatic modes",
      "Parameter settings",
      "Jogging mode",
      "Format change procedure"
    ],
    "maintenance": [
      "Lubrication schedule",
      "Preventive checks",
      "Part replacements",
      "Handling hot surfaces"
    ],
    "troubleshooting": [
      "Common error indicators",
      "Safety interlock checks",
      "Emergency shutdown",
      "Corrective actions"
    ],
    "electronic_controls": [
      "Operator panel overview",
      "Parameter masks",
      "Electronic cam switching",
      "User management"
    ],
    "process_control": [
      "Filling inspection",
      "Foil registration",
      "Barcode scanning",
      "Product rejection logic"
    ]
  },

  "query_relevance_classifier": {
    "how_to_change_film_on_blister_machine": True,
    "safety_protocols_for_machine_operators": True,
    "cooling_water_system_operation": True,
    "use_for_medical_blister_packs": True,
    "troubleshooting_forming_defects": True,
    "operate_tablet_press": False,
    "build_packaging_machine": False,
    "history_of_blister_packaging": False
  }
}

SUMMARY = """ Document Summary: Blister Machine N 921 – Operating Instructions
This document is the official operating manual for the Romaco Noack Blister Machine N 921, used primarily in pharmaceutical packaging to form and seal blister packs. It is intended for trained personnel responsible for the machine's setup, operation, maintenance, and troubleshooting.

Main Topics Covered:
Technical Data: Specifications including power supply, dimensions, film types, and performance (e.g., max cycles per minute).

Safety Instructions: Mandatory safety protocols, hazard symbols, PPE requirements, and operational precautions (e.g., electrical, pneumatic, thermal, and mechanical risks).

Machine Setup: Installation prerequisites (e.g., room conditions, floor leveling), transport procedures, and electrical/compressed air/cooling connections.

Machine Description: Detailed explanations of each module (e.g., forming station, feeding system, sealing station, die-cutting unit, electronic control panel) and their roles in the packaging process.

Operation Procedures: Start-up, format change, parameter setting, feeding, inspection, sealing, coding, cutting, and discharging.

Maintenance & Troubleshooting: Preventive checks, component handling, warning sign locations, and steps to identify and fix common machine faults.

Electronic Control: User interface layout, parameter masks, format management, and control logic via electronic cam switching units.

Intended Use Context:
This manual is highly domain-specific, centered on the mechanical, electrical, and procedural aspects of industrial blister packaging operations. It assumes technical background in machine operation and engineering principles."""

class SPANGPT():

    def __init__(self):
        load_dotenv()
        self.client = OpenAI(api_key = os.getenv("OPENAI_API_KEY"))
        self.conversation_history = {}  # Store recent conversations with summaries
        self.max_history_length = 5  # Number of exchanges to remember
        self.previous_answer = ""
        self.previous_query = ""
        self.total_5_previous_question_list = []
        self.max_attempt = 3

    def SummarizeResponse(self, user_query, assistant_response):
        """Generate a summary of the current conversation exchange"""
        try:
            # Generate summary using OpenAI
            summary_response = self.client.chat.completions.create(
                model=MODEL_NAME,
                messages=[
                    {
                        "role": "system",
                        "content": """
                        
                        You are a specialized summarizer tasked with condensing assistant Response while preserving essential information. Your goal is to create compact yet informative versions of responses that can be efficiently stored in conversation history.
                        Summary Requirements

                        1 ] Maintain core information integrity:

                         - Preserve the main points and key details of the original Response
                         - Keep critical facts, figures, and specific answers to user Query
                         - Retain any commitments, recommendations, or direct answers

                        2 ] Preserve exact structural elements:

                         - Maintain EXACT step numbers as they appear in the original (e.g., "Step 1", "Step 2")
                         - Keep EXACT point numbers as they appear (e.g., "1.", "2.", "a.", "b.")
                         - Preserve EXACT titles, headings, and section names without modification
                         - Retain the exact order and hierarchy of numbered/bulleted items

                        3 ] Remove non-essential elements:

                         - Eliminate redundant information and unnecessary repetition
                         - Remove excessive examples when multiple are provided
                         - Cut verbose explanations while keeping conclusions
                         - Trim introductory phrases, pleasantries, and transition text

                        4 ] Preserve context relevance:

                         - Maintain references to previous questions or conversation context
                         - Keep information that might be referenced in future exchanges
                         - Preserve any clarification of user intent or query interpretation

                        5 ] Format efficiently:

                         - Use concise phrasing and remove filler words
                         - Convert lengthy paragraphs to more compact forms where appropriate
                         - Maintain a professional, helpful tone in simplified language
                         - Aim for 25-50% of the original length, depending on content density

                        6 ] Special content handling:

                         - For lists: Keep headings but limit examples or explanations
                         - For instructions: Maintain all steps but condense explanations
                         - For technical information: Preserve all technical details but condense surrounding text
                         - For code examples: Keep them intact or include only the most essential parts

                        7 ] Process

                         - Review the complete assistant Response
                         - Identify the core information and essential details
                         - Eliminate redundancies and non-critical elements
                         - Restructure as needed for conciseness
                         - Verify that the summary accurately represents the original Response
                         - Ensure all direct answers to user Query are preserved

                        8 ] Output Format
                         - Return only the summarized version of the assistant's response without any meta-commentary about the summarization process. Do not include phrases like "Here's a summary" or "In summary." Simply provide the condensed response text directly.

                        Critical Preservation Rules
                         - NEVER renumber or resequence points, steps, or list items
                         - NEVER modify, rephrase, or abbreviate headings, titles, or section names
                         - NEVER change the structural format of numbered or bulleted lists
                         - When condensing underneath a numbered point or heading, only reduce the explanatory text

                        """
                    },
                    {
                        "role": "user",
                        "content": f"Query: {user_query}\nResponse: {assistant_response}"
                    }
                ],
                temperature=0,
                top_p=0
            )
            
            # Extract summary text from response
            if hasattr(summary_response, 'choices') and len(summary_response.choices) > 0:
                return summary_response.choices[0].message.content
            return "No summary generated"
            
        except Exception as e:
            print(f"Error generating summary: {str(e)}")
            return "Error generating summary"

    def FilterOutStartingConversation(self, user_query, user_id = 1):

        """
        
        Starting conversation, Greetings, Make it short/consice/allabarate 

        """ 

        print("*"*90)
        print(self.total_5_previous_question_list)
        print("*"*90)

        print(self.conversation_history)

        # a = input("Input ~~~~~~~~~~~~~~~~~")

        # Prepare conversation history context
        # history_context = ""
        # if user_id in self.conversation_history and self.conversation_history[user_id]:
        #     history_context = "Previous conversation exchanges:\n"
        #     for i, exchange in enumerate(self.conversation_history[user_id], 1):
        #         history_context += f"Exchange {i}: {exchange['summary']}\n"

        history_context = ""
        if user_id in self.conversation_history and self.conversation_history[user_id]:
            history_context = "Previous conversation exchanges:\n"
            for i, exchange in enumerate(self.conversation_history[user_id], 1):
                history_context += f"Exchange {i}: Question: {exchange['user']} | Answer: {exchange['summary']}\n"

        # history_context = self.conversation_history

        print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ history_context ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~")
        print(history_context)
        print("~"*90)
        
        completion = self.client.chat.completions.create(
            model=MODEL_NAME,
            messages=[
                {
                    "role": "system",
                    "content": """
                    You are a Query classification assistant. Your job is to determine whether to proceed to the next filter or not based on the user's Query.
                    
                    Analyze the user's Query and Conversation History to determine:
                    
                    1. If the Query is a greeting:
                    - Return {"do_similarity_search": "False", "message": "Hello! I'm your document assistant for the Blister Machine N 921 manual. How can I help you with information about this machine?"}
                    
                    2. If the Query is regarding "make it shorter", "summarize this", "give me only the first 3 points", "what are the main points", "make it more concise", etc.
                    * IMPORTANT: Only process this if the query is clearly asking to modify/reformat the previous response
                    * You must actually process and modify the Previous Answer text according to the request:
                        - For summarization requests: Create a concise version modified_content that captures the key points but is significantly shorter.
                        - For requests for specific portions ("only first 3 points", "just the last part", etc.): Identify and extract only those specific portions and make modified_content.
                        - For requests to focus on specific aspects: Extract only the relevant portions that address those aspects and make modified_content.
                    * The modified_content must be the actual processed/modified version of the Previous Answer.
                    * Return {"do_similarity_search": "False", "message": modified_content}

                    3. If the Query is a counter query or follow-up question related to the previous query:
                    * IMPORTANT: This includes requests that seek additional information, clarification, or alternative perspectives related to the previous query and answer.
                    * Examples include: "What about [related aspect]?", "Can you explain more about [specific part mentioned]?", "Is there an alternative to [previously discussed topic]?", "I disagree with [previous point], what else?", etc.
                    * Return {"do_similarity_search": "True", "message": "Do Next Filter"}
                    
                    4. For all other document-related queries:
                    - Return {"do_similarity_search": "True", "message": "Do Next Filter"}

                    5. If you don't know the answer to the query:
                    - Return {"do_similarity_search": "True", "message": "Do Next Filter"}
                    - Do not attempt to create or fabricate an answer.
                    
                    Return ONLY a valid JSON object with the format {"do_similarity_search": "boolean", "message": string}.
                    Do not include any other text or explanation in your response.
                    """
                },
                {
                    "role": "user",
                    "content": f"Query: '{user_query}'\nConversation History: {history_context}\nPrevious Answer: {self.previous_answer}\nPrevious Query: {self.previous_query}"
                }
            ],
            top_p = 0.00000001,
            temperature=0,
            response_format={"type": "json_object"}
        )
        
        return completion.choices[0].message.content, history_context


    def FilterOutQuery(self, user_query, history_context, user_id = 1):


        """ 
        When counter Query is regarding elaborate prevoius answer. 

        """
        
        completion = self.client.chat.completions.create(
            model=MODEL_NAME,
            messages=[
                {
                    "role": "system",
                    "content": """
                    You are a Query classification assistant. Your job is to determine if a similarity search should be performed for the user's Query.
                    
                    Analyze the user's Query, Conversation History, Previous Answer and Previous Question List to determine:

                    1. If the Query is asking to elaborate, expand, or provide more detail about a previous answer:
                        - If the model determines the query is requesting elaboration of a previous answer:
                            * Extract ONLY the key context from the Conversation History.
                            * Construct a concise new query that captures the essence of the follow-up question.
                            * Do NOT include the entire previous answer in the expanded query.
                            * Keep the expanded query brief and focused on the specific information requested.
                            * Include only relevant keywords from previous context, not entire passages.
                            * Make sure the expanded query is directly relevant to both the current Query and the past queries found in the Conversation History.
                            * Do not add any extra text, formatting, or explanations to the expanded_query.
                        - Return {"do_similarity_search": "True", "message": expanded_query}

                    2. If the Query is related to the document content and requires searching the document but does not depend on previous queries:
                    - Return {"do_similarity_search": "True", "message": original_query} for clear queries regardless of length
                    - Do not add new questions or change the core intent of the original Query

                    3. If the Query is a follow-up question or references previous conversation from the Conversation History:
                       - Use the Conversation History to understand the context
                       - Formulate a brief, self-contained query that captures the essential intent
                       - Limit the query to 2-3 sentences focusing only on the new information needed
                       - Avoid repeating information that was already provided in previous answers
                       - Do not add new questions or change the core intent of the original Query
                       - Return {"do_similarity_search": "True", "message": self_contained_query}

                    4. Else :
                     - Return {"do_similarity_search": "True", "message": original_query}
                    
                    Return ONLY a valid JSON object with the format {"do_similarity_search": "boolean", "message": string}.
                    Do not include any other text or explanation in your response.

                    * IMPORTANT GUIDELINES FOR GENERATING QUERIES:

                     - Keep all generated queries concise and to the point
                     - Never include the entire previous answer text in the new query
                     - Focus only on the new information being requested
                     - Maximum length for generated queries should be 3-4 sentences
                     - Extract only essential context from previous conversations, not full text

                    """
                },
                {
                    "role": "user",
                    "content": f"Query: '{user_query}'\nConversation History: {history_context}\nDocument Metadata: {META_DATA}\nDocument Summary: {SUMMARY}\nPrevious Answer: {self.previous_answer}\nPrevious Query: {self.previous_query}\nPrevious Question List: {self.total_5_previous_question_list}"
                }
            ],
            top_p = 0.00000001,
            temperature=0,
            response_format={"type": "json_object"}
        )
        
        return completion.choices[0].message.content




    # def FilterOutQuery(self, user_query, history_context, user_id = 1):


    #     """ 
    #     When counter Query is regarding elaborate prevoius answer. 

    #     """
        
    #     completion = self.client.chat.completions.create(
    #         model=MODEL_NAME,
    #         messages=[
    #             {
    #                 "role": "system",
    #                 "content": """
    #                 You are a Query classification assistant. Your job is to determine if a similarity search should be performed for the user's Query.
                    
    #                 Analyze the user's Query, Conversation History, Previous Answer and Previous Question List to determine:

    #                 1. If the Query is asking to elaborate, expand, or provide more detail about a previous answer:
    #                     - If the model determines the query is requesting elaboration of a previous answer:
    #                         * Extract ONLY the key context from the Conversation History.
    #                         * Construct a concise new query that captures the essence of the follow-up question.
    #                         * Do NOT include the entire previous answer in the expanded query.
    #                         * Keep the expanded query brief and focused on the specific information requested.
    #                         * Include only relevant keywords from previous context, not entire passages.
    #                         * Make sure the expanded query is directly relevant to both the current Query and the past queries found in the Conversation History.
    #                         * Do not add any extra text, formatting, or explanations to the expanded_query.
    #                     - Return {"do_similarity_search": "True", "message": expanded_query}

    #                 2. If the Query is related to the document content and requires searching the document but does not depend on previous queries:
    #                 - Return {"do_similarity_search": "True", "message": original_query} for clear queries regardless of length
    #                 - Do not add new questions or change the core intent of the original Query

    #                 3. If the Query is a follow-up question or references previous conversation from the Conversation History:
    #                    - Use the Conversation History to understand the context
    #                    - Formulate a brief, self-contained query that captures the essential intent
    #                    - Limit the query to 2-3 sentences focusing only on the new information needed
    #                    - Avoid repeating information that was already provided in previous answers
    #                    - Do not add new questions or change the core intent of the original Query
    #                    - Return {"do_similarity_search": "True", "message": self_contained_query}

    #                 4. For Query processing and gibberish detection:
    #                     - **GENERAL PRINCIPLE:** If a query contains ANY meaningful English words or phrases that form a question or request, it should be treated as valid regardless of any other characters present
                        
    #                     - **Process for analyzing queries:**
    #                     1. Examine the entire query for any recognizable English words, phrases, questions, or meaningful content
    #                     2. If meaningful content exists (even partially), extract it and clean up excessive punctuation/symbols
    #                     3. Only consider a query as gibberish if it contains NO recognizable English words or coherent phrases
    #                     4. Common shortcuts (r=are, u=you, etc.) and informal language should be recognized as valid
    #                     5. Multiple punctuation marks (!!, ??, etc.) are stylistic choices and don't make a query invalid
    #                     6. Ignore random characters interspersed with valid text - focus on extracting the meaningful content
                        
    #                     - **Core algorithmic approach:**
    #                     * Split the query into tokens and identify any English words or common text shortcuts
    #                     * If English words form a coherent question or request (even incomplete), process it
    #                     * Focus on the communicative intent rather than perfect formatting
                        
    #                     - If gibberish is detected (NO meaningful words found): Return {"do_similarity_search": "False", "message": "Your message appears to contain random characters or no meaningful content. Please provide a clear question related to the document."}
                        
    #                     - If ANY meaningful content is found: Extract the core question by removing excessive punctuation and random characters while preserving the original intent, then proceed with similarity search
                    
    #                 Return ONLY a valid JSON object with the format {"do_similarity_search": "boolean", "message": string}.
    #                 Do not include any other text or explanation in your response.

    #                 * IMPORTANT GUIDELINES FOR GENERATING QUERIES:

    #                  - Keep all generated queries concise and to the point
    #                  - Never include the entire previous answer text in the new query
    #                  - Focus only on the new information being requested
    #                  - Maximum length for generated queries should be 3-4 sentences
    #                  - Extract only essential context from previous conversations, not full text
    #                  - When cleaning up queries, preserve the original intent and meaning
    #                  - Remove excessive punctuation but maintain the core question

    #                 """
    #             },
    #             {
    #                 "role": "user",
    #                 "content": f"Query: '{user_query}'\nConversation History: {history_context}\nDocument Metadata: {META_DATA}\nDocument Summary: {SUMMARY}\nPrevious Answer: {self.previous_answer}\nPrevious Query: {self.previous_query}\nPrevious Question List: {self.total_5_previous_question_list}"
    #             }
    #         ],
    #         top_p = 0.00000001,
    #         temperature=0,
    #         response_format={"type": "json_object"}
    #     )
        
    #     return completion.choices[0].message.content

    def DoSimilarity(self, refine_query, history_context, user_id = 1):
        
        results = self.client.vector_stores.search(
            vector_store_id="vs_67d96b94416481919d4bcc449534816a",
            query=refine_query, 
            max_num_results = 5
            #top_k=5, 
        )

        # Prepare to collect assistant's response
        full_response = ""

        gpt_context = '\n'.join(result.content[0].text for result in results.data)

        completion = self.client.chat.completions.create(
        model=MODEL_NAME,
        messages=[
            {
                "role": "system",
                "content": """
                    - First, carefully analyze if the user's Query can be directly answered using ONLY the provided Sources.
                    - Do not use your own knowledge to answer if you cannot find the answer in the provided context.
                    - If the document mentions keywords from the query but does not contain a specific answer, clearly state this limitation: "While the document mentions [relevant keywords], it does not provide specific information about [query topic]."

                    - CRITICAL: Carefully evaluate if the current Query stands independently from the History_context:
                    * If the Query contains a complete, standalone question that doesn't reference previous exchanges
                    * If the Query introduces a new topic unrelated to recent conversation
                    * If the Query is asking for specific information that can be fully answered without History_context
                    * If the Query is about technical information, procedures, or factual content
                    THEN: Ignore History_context completely and provide a full, comprehensive response based ONLY on the Sources.

                    - Only use History_context summaries when the current Query explicitly references previous exchanges or is a direct follow-up that cannot be understood without context.
                    - When using History_context, prioritize the most recent exchanges.
                    - For queries requiring detailed explanation, provide only information found in the context in a step-by-step manner.
                    - Always maintain the exact sequence of steps as presented in the source material without altering the order.
                    - For greeting or general conversational queries, respond as a friendly chatbot without referencing the document context.
                    - If there is absolutely no relevant information in the provided sources, respond only with: "Provided query is out of context. Please ask query related to document."
                    - Maintain complete consistency in responses for identical queries.
                    - Do not introduce variations in your responses for the same input.
                    - Never attempt to infer, guess, or add information that isn't explicitly stated in the provided sources.

                    - IMPORTANT: At the end of your response, include a "Reference" section that lists all page numbers used to formulate your answer.
                    - Format references as:
                    
                        Reference:
                        Page No. X, Y, Z
                        
                        - If information comes from consecutive pages, use a range format:
                        
                        Reference:
                        Page No. X-Z
                        
                        - Do not include this reference section if no specific page numbers are available in the sources.

                """
            },
            {
                "role": "user",
                "content": f"History_context : {history_context}\n\nSources: {gpt_context}\n\nQuery: '{refine_query}'"
            }
        ],
        top_p = 0.00000001,
        temperature=0,
        # stream=True,
    )

        print("="*90)
        print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> GPT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ")
        print("="*90)

        # for chunk in completion:
        #     if chunk.choices[0].delta.content is not None:
        #         print(chunk.choices[0].delta.content, end="")
        #         content_chunk = chunk.choices[0].delta.content
        #         full_response += content_chunk

        # self.previous_answer = full_response

        # self.previous_answer = completion.choices[0].message.content
        
        # After collecting full response, update conversation history with summary
        # self.UpdateConversationHistory(refine_query, full_response, user_id = 1)
        return completion.choices[0].message.content

    # def DoSimilarity(self, refine_query, history_context, user_id = 1):
        
    #     results = self.client.vector_stores.search(
    #         vector_store_id="vs_67d96b94416481919d4bcc449534816a",
    #         query=refine_query, 
    #         max_num_results = 5
    #         #top_k=5, 
    #     )

    #     # Prepare to collect assistant's response
    #     full_response = ""

    #     gpt_context = '\n'.join(result.content[0].text for result in results.data)

    #     completion = self.client.chat.completions.create(
    #         model=MODEL_NAME,
    #         messages=[
    #             {
    #                 "role": "system",
    #                 "content": """
    #                             - Understand the user Query and Source then answer accordingly.
    #                             - Only use History_context summaries when necessary to maintain context.
    #                             - If the question is not related to History_context, do not follow the history at all and provide the answer.
    #                             - Only use History_context when you feel like History is important to provide the answer.
    #                             - When using History_context, give more importance to the most recent exchanges.
    #                             - If answer requires detailed explanation, provide as much detail as possible from the context provided in a step-by-step manner.
    #                             - When providing detailed answers, maintain the exact sequence of steps as presented in the source material without changing the order.
    #                             - If the context contains information related to the Query, even partially, use that information to construct a helpful response.
    #                             - If Query is related to greetings then do not follow the context. Reply as a friendly chatbot.
    #                             - If there is absolutely no relevant information in the provided sources, only say "Provided query is out of context. Please ask query related to document."
    #                             - Always provide the exact same response for identical queries to ensure consistency.
    #                             - Do not introduce randomness or variations in your responses for the same input.
    #                 """
    #             },
    #             {
    #                 "role": "user",
    #                 "content": f"History_context : {history_context}\n\nSources: {gpt_context}\n\nQuery: '{refine_query}'"
    #             }
    #         ],
    #         top_p = 0.00000001,
    #         temperature=0,
    #         # stream=True,
    #     )

    #     print("="*90)
    #     print(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> GPT >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ")
    #     print("="*90)

    #     # for chunk in completion:
    #     #     if chunk.choices[0].delta.content is not None:
    #     #         print(chunk.choices[0].delta.content, end="")
    #     #         content_chunk = chunk.choices[0].delta.content
    #     #         full_response += content_chunk

    #     # self.previous_answer = full_response

    #     # self.previous_answer = completion.choices[0].message.content
        
    #     # After collecting full response, update conversation history with summary
    #     # self.UpdateConversationHistory(refine_query, full_response, user_id = 1)
    #     return completion.choices[0].message.content

    def Reviewer(self, user_query, gpt_answer, history_context, user_id = 1):
        completion = self.client.chat.completions.create(
            model=MODEL_NAME,
            messages=[
                {
                    "role": "system",
                    "content": """

                        You are a specialized evaluator tasked with determining whether a GPT Answer is relevant to a user's Query while considering Conversation History. Your sole responsibility is to assess if the response appropriately addresses the query's intent and information needs.
                        Evaluation Instructions

                        1 ] Analyze the user Query carefully:

                        - Identify the main question or request
                        - Recognize any explicit constraints or requirements
                        - Note any implicit information needs

                        2 ] Review the GPT answer thoroughly:

                         - Check if it directly addresses the main question/request
                         - Verify if it satisfies both explicit and implicit information needs
                         - Evaluate if the level of detail is appropriate

                        3 ] Consider the Conversation History:

                         - Examine if the answer maintains appropriate contextual continuity
                         - Check if it builds upon previously established information
                         - Verify it doesn't contradict previously shared information

                        4 ] Determine contextual relevance:

                         - The answer should use relevant information from provided sources
                         - The answer should not include excessive irrelevant information
                         - The answer should focus on the specific Query rather than tangential topics

                        5 ] Relevance Criteria

                        -  Mark the response as "Not Ok" if ANY of these conditions are met:

                         - The response fails to address the primary intent of the Query
                         - The response contradicts information from Conversation History without explanation
                         - The response ignores critical contextual information from Conversation History
                         - The response contains mostly irrelevant information that doesn't help answer the Query
                         - The response answers a different question than what was asked
                         - The response deliberately avoids addressing the Query by providing generic information

                        6 ] Mark the response as "Ok" if ALL of these conditions are met:

                         - The response directly addresses the core intent of the user Query
                         - The response is consistent with previous Conversation History
                         - The response provides information that is helpful for the specific question asked
                         - The response maintains appropriate context from the Conversation History
                         - The response includes relevant information from sources when applicable

                        7 ] Output Format
                         - Provide your evaluation in the following JSON format without any additional text:
                         - json{
                            "message": "Ok" OR "message": "Not Ok"
                            }

                        * Example Evaluation Process

                         - User Query: "What were the financial results for Q2?"
                         - GPT Answer: "The company's environmental initiatives include solar panels and waste reduction."
                         - History: Previous conversation about company financial reports
                         - Evaluation: The answer discusses environmental initiatives instead of financial results → {"message": "Not Ok"}

                            OR

                         - User Query: "What were the financial results for Q2?"
                         - GPT Answer: "In Q2, the company reported revenue of $12.5M, up 4% YoY, with an EBITDA of $3.2M."
                         - History: Previous conversation about company financial reports
                         - Evaluation: The answer directly addresses the financial results requested → {"message": "Ok"}

                    Return ONLY a valid JSON object with the format {"message": "Ok"}.
                    Do not include any other text or explanation in your response.

                    """
                },
                {
                    "role": "user",
                    "content": f"Conversation History : {history_context}\n\GPT Answer: {gpt_answer}\n\nQuery: '{user_query}'"
                }
            ],
            top_p = 0.00000001,
            temperature=0,
            response_format={"type": "json_object"}
        )

        return completion.choices[0].message.content


    # def Reviewer(self, user_query, gpt_answer, history_context, user_id = 1):
    #     completion = self.client.chat.completions.create(
    #         model=MODEL_NAME,
    #         messages=[
    #             {
    #                 "role": "system",
    #                 "content": """

    #                 You are a specialized evaluator tasked with determining whether a GPT Answer is relevant to a user's Query while considering Conversation History and available Document Metadata and Summaries. Your
    #                 responsibility is to make nuanced evaluations about response appropriateness.
                    
    #                 Core Evaluation Principles
                    
    #                 1. Understand the document scope first:
    #                  - Before evaluating anything else, clearly identify what topics and information are covered in the Document Metadata and Summary
    #                  - Recognize the specific domain and limitations of the knowledge provided in the documents
    #                  - Consider the documents as the authoritative boundary for what can be answered
                    
    #                 2. Evaluate query scope appropriately:
    #                  - A query is "in-scope" if it relates to information contained in the documents or directly builds on conversation history
    #                  - A query is "out-of-scope" if it requests information entirely unrelated to document content or previous conversation
                    
    #                 3. Recognize appropriate handling of out-of-scope queries:
    #                  - When a query is out-of-scope, a GOOD response should:
    #                   - Clearly indicate the query is outside the available knowledge domain
    #                   - Optionally redirect to topics that ARE within scope
    #                   - NOT attempt to provide potentially fabricated information on out-of-scope topics
                    
    #                  - Such responses should be marked as "Ok" even though they don't provide the requested information
                    
    #                 Detailed Evaluation Instructions
                    
    #                 Query and Response Analysis
                    
    #                 1. Analyze the document context thoroughly:
                    
    #                  - Review the Document Metadata and Summary to understand the information boundaries
    #                  - Identify the specific domain, topics, and scope of the available information
                    
    #                 2. Assess query relevance to available context:
                     
    #                  - Determine if the query relates to topics covered in document context
    #                  - Check if the query builds on information in conversation history
    #                  - Identify if answering would require knowledge outside available information
                    
    #                 3. Review response appropriateness:
                    
    #                  - For in-scope queries: Check if the response provides relevant information from the documents
    #                  - For out-of-scope queries: Verify the response appropriately declines to provide information while explaining why
    #                  - For ambiguous queries: Ensure the response makes reasonable interpretation based on document context
                    
    #                 4. Check response consistency with conversation history:
                    
    #                  - Verify the response maintains contextual continuity
    #                  - Ensure it doesn't contradict previously established information
    #                  - Confirm it builds appropriately on previous exchanges
                    
    #                 Specific Relevance Criteria
                    
    #                  - Mark as "Not Ok" if:
                    
    #                     - The query is in-scope but the response fails to provide relevant information
    #                     - The query is out-of-scope and the response attempts to provide potentially fabricated information
    #                     - The response contradicts document content or conversation history
    #                     - The response incorrectly claims a query is out-of-scope when relevant information exists in the documents
    #                     - The response provides excessive irrelevant information that distracts from addressing the query
    #                     - The response ignores critical context from previous conversation

    #                  - Mark as "Ok" if:

    #                     - The query is in-scope and the response provides relevant information from the documents
    #                     - The query is out-of-scope and the response appropriately indicates this limitation
    #                     - The response maintains consistent context with previous conversation
    #                     - The response provides helpful clarification or redirection when appropriate
    #                     - The response correctly balances depth and brevity based on query complexity

    #                 Output Format
    #                 Provide your evaluation in the following JSON format without any additional text:

    #                 {
    #                 ""message"": "Ok" OR ""message"": ""Not Ok""
    #                 }

    #                 Return ONLY a valid JSON object with the format {"message": "Ok"} or {"message": "Not Ok"}. Do not
    #                 include any other text or explanation in your response.

    #                 """
    #             },
    #             {
    #                 "role": "user",
    #                 "content": f"Conversation History : {history_context}\n\GPT Answer: {gpt_answer}\n\nQuery: '{user_query}\nDocument Metadata: {META_DATA}\nDocument Summary: {SUMMARY}'"
    #             }
    #         ],
    #         top_p = 0.00000001,
    #         temperature=0,
    #         response_format={"type": "json_object"}
    #     )

    #     return completion.choices[0].message.content

    def AskQuery(self, user_query, user_id = 1):

        filter_starting_conv, history_context = self.FilterOutStartingConversation(user_query, user_id = 1)
        filter_starting_conv = eval(filter_starting_conv)
        print(filter_starting_conv)

        # a = input("input <><><><><><>")

        if filter_starting_conv["do_similarity_search"] == "True":
            if filter_starting_conv["message"] == "Do Next Filter":
                print(">>> DO NEXT FILTER")
                filter_response = self.FilterOutQuery(user_query, history_context, user_id = 1)
                filter_response = eval(filter_response)
                print(filter_response)

                if filter_response["do_similarity_search"] == "False":
                    print(filter_response["message"])
                    # self.UpdateConversationHistory(user_query, filter_response["message"], user_id = 1)
                    return user_query, filter_response["message"], history_context
                refine_query = filter_response["message"]
                return refine_query, self.DoSimilarity(refine_query, history_context), history_context

            else:
                print(filter_starting_conv["message"])
                new_query = filter_starting_conv["message"]
                return new_query, self.DoSimilarity(new_query, history_context), history_context
                # return True

        if filter_starting_conv["do_similarity_search"] == "False":
            print(filter_starting_conv["message"])
            # self.UpdateConversationHistory(user_query, filter_starting_conv["message"], user_id = 1)
            return user_query, filter_starting_conv["message"], history_context

    def UpdateConversationHistory(self, user_query, assistant_response, user_id):
        """Update the conversation history with summaries, maintaining only the last 5 exchanges"""

        if assistant_response == "Please provide a question regarding the document." or assistant_response == "Provided query is out of context. Please ask query related to document." or assistant_response == "Your message appears to contain random characters or no meaningful content. Please provide a clear question related to the document.":
            pass
        else:
            # Generate summary of current exchange
            exchange_summary = self.SummarizeResponse(user_query, assistant_response)

            if user_id not in self.conversation_history:
                self.conversation_history[user_id] = []
            
            # Add current exchange summary to history
            self.conversation_history[user_id].append({
                'user': user_query,
                'assistant': assistant_response,
                'summary': exchange_summary
            })
            
            # If we exceed maximum history length, remove oldest conversation
            if len(self.conversation_history[user_id]) > self.max_history_length:
                if self.conversation_history[user_id] != []:
                    self.conversation_history[user_id].pop(0)  # Remove oldest exchange
            if len(self.total_5_previous_question_list) > self.max_history_length:
                if self.total_5_previous_question_list != []:
                    self.total_5_previous_question_list.pop(0)

    def Main(self, user_query, user_id = 1):

        print("^_+_^"*30)
        print("user_query : ", user_query)
        print("^_+_^"*30)
        attempt = 0
        while attempt < self.max_attempt:
            query, gpt_answer, history_context = self.AskQuery(user_query, user_id = 1)
            print("^_+_^"*30)
            print("query : ", query)
            print("!"*90)
            print(gpt_answer)
            print("^_+_^"*30)
            review = self.Reviewer(user_query, gpt_answer, history_context, user_id = 1)
            print(review)
            review = eval(review)
            if review["message"] == "Ok":
                self.previous_query = query
                self.previous_answer = gpt_answer
                self.total_5_previous_question_list.append(self.previous_query)
                self.UpdateConversationHistory(query, gpt_answer, user_id = 1) 
                break
            else:
                attempt += 1

            print("_-_-_-"*50)
            print(attempt)
            print("_-_-_-"*50)

            # a = input("):):):):):):):):):): input")

        return gpt_answer

# if __name__=="__main__":
#     user_query = "how to do product changeover of blister packaging machine?"
#     span_gpt_obj = SPANGPT()
#     while True:
#         print(" ")
#         user_query = input("Ask Me Anything : ")
#         if user_query == "q":
#             break
#         # span_gpt_response, _ = span_gpt_obj.AskQuery(user_query)
#         span_gpt_response = span_gpt_obj.Main(user_query)
#         print("+"*90)
#         print(span_gpt_response)
#         print("+"*90)

#########################################################################################

#  file_id='file-BfhfL6sLisriJHJKkSkach'
# vector_store_id="vs_67d91fcd5cd88191976f9520b5a5d1ff",         # Default Model

#########################################################################################

# vs_67d95d31fff08191bcb8b96e2ae2b553   # 1000 200   Support FAQ 456

# vs_67d96b94416481919d4bcc449534816a    # 1000 200 direct passing the pdf file

app = FastAPI()
span_gpt_obj = SPANGPT()

class InferenceData(BaseModel):
    query: str

@app.post("/execute_span_gpt_inference/")
async def execute_span_gpt_inference(request: InferenceData):
    user_query = request.query
    print("Query: ", user_query)
    result = span_gpt_obj.Main(user_query)
    ascii_text = result.encode('ascii', 'ignore').decode('ascii')
    print("RESPONSE: ", result)
    return ascii_text

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=9094)